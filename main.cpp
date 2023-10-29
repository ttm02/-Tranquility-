#include <iostream>

#include <utility>
#include <vector>
#include <memory>

#include <algorithm>
#include <random>
#include <iomanip>
#include <cassert>
#include <limits>

//TODO implement special rules for less players
#define MIN_PLAYER_COUNT 3
#define MAX_PLAYER_COUNT 5

#define NUM_DISCARD_DISCARD_PHASE 8

class Card {
public:
    const unsigned int value;

    // max value is inclusive
    static const unsigned int MAX_VALUE = 80;

    // special value
    static const unsigned int START = 100;

    static const unsigned int FINISH = 101;


    explicit Card(unsigned int v) : value(v) {};

    // not copy able
    Card(const Card &) = delete;

    // default destructor

};

const unsigned int Card::MAX_VALUE;
const unsigned int Card::START;
const unsigned int Card::FINISH;


std::vector<std::unique_ptr<Card>> create_deck() {
    std::vector<std::unique_ptr<Card>> deck;

    deck.reserve(100);
    for (int i = 0; i < 5; ++i) {
        //deck.push_back(std::make_unique<Card>(Card::START));// each player gets one start card
        deck.push_back(std::make_unique<Card>(Card::FINISH));
    }
    for (unsigned int i = 0 + 1; i < Card::MAX_VALUE + 1; ++i) {
        deck.push_back(std::make_unique<Card>(i));
    }

    return deck;

};

class GameManager;

//TODO refactor; the play area is not 2d it is just presented as 2d
class PlayArea {
public:
    PlayArea() {
        for (int i = 0; i < 6; ++i) {
            area.push_back(std::vector<std::unique_ptr<Card>>(6));
            // nullptr is default
        }
    }

    PlayArea(PlayArea &) = delete;

private:
    std::vector<std::vector<std::unique_ptr<Card>>> area;
public:
    const std::vector<std::vector<std::unique_ptr<Card>>> &get_area() const { return area; };

    void play_card(std::pair<int, int> pos, std::unique_ptr<Card> card) {

        assert(card != nullptr);
        assert(get_num_discard(pos, card->value) != -1);

        if (card->value == Card::START) {
            has_start_ = true;
            return;
        }
        if (card->value == Card::FINISH) {
            has_finish_ = true;
            return;
        }
        area[pos.first][pos.second] = std::move(card);
        return;
    }

    // return -1 if move is invalid
    int get_num_discard(std::pair<int, int> pos, unsigned int new_card_value) const {
        if (new_card_value == Card::START) {
            // check if valid
            if (not has_start_) {
                return 0;
            }
            {
                return -1;
            }
        }
        if (new_card_value == Card::FINISH) {
            auto num_gaps = std::accumulate(area.begin(), area.end(), 0, [](auto accu, const auto &row) {
                return accu + std::accumulate(row.begin(), row.end(), 0, [](auto accu, const auto &card) {
                    if (card == nullptr) {
                        return accu + 1;
                    } else {
                        return accu + 0;
                    }
                });
            });
            if (not has_finish_ && num_gaps == 0 && has_start_) {
                return 0;
            } else {
                return -1;
            }
        }

        // position is in Boundaries and free
        if (pos.first >= 0 && pos.first < HEIGHT && pos.second >= 0 && pos.second < WIDTH &&
            area[pos.first][pos.second] == nullptr) {

            std::pair<int, int> left_neighbor = pos;
            if (pos != std::make_pair(0, 0)) {
                if (pos.second == 0) {
                    left_neighbor = std::make_pair(pos.first - 1, WIDTH);
                } else {
                    left_neighbor = std::make_pair(pos.first, pos.second - 1);
                }
            }
            std::pair<int, int> right_neighbor = pos;
            if (pos != std::make_pair(5, 5)) {
                if (pos.second == WIDTH - 1) {
                    left_neighbor = std::make_pair(pos.first + 1, 0);
                } else {
                    left_neighbor = std::make_pair(pos.first, pos.second + 1);
                }
            }
            if (area[right_neighbor.first][right_neighbor.second] == nullptr &&
                area[left_neighbor.first][left_neighbor.second] == nullptr) {
                // no neighbor
                return 0;
            } else {
                unsigned int num_discard = std::numeric_limits<int>::max();

                if (area[right_neighbor.first][right_neighbor.second] != nullptr) {
                    if (area[right_neighbor.first][right_neighbor.second]->value < new_card_value) {
                        return -1;
                    }
                    num_discard = std::min(num_discard,
                                           area[right_neighbor.first][right_neighbor.second]->value - new_card_value);
                }
                if (area[left_neighbor.first][left_neighbor.second] != nullptr) {
                    if (new_card_value < area[left_neighbor.first][left_neighbor.second]->value) {
                        return -1;
                    }
                    num_discard = std::min(new_card_value - num_discard,
                                           area[left_neighbor.first][left_neighbor.second]->value);
                }
                assert(num_discard <= std::numeric_limits<int>::max());
                return static_cast<int>(num_discard);
            }

        } else {
            return -1; // invalid
        }
    }

private:
    bool has_start_ = false;
    bool has_finish_ = false;
public:
    bool has_start() const { return has_start_; }

    bool has_finish() const { return has_finish_; }

    static const unsigned int WIDTH = 6;
    static const unsigned int HEIGHT = 6;

    void print() const {
        // first row
        if (has_start_) {
            std::cout << " S";
        } else {
            std::cout << "  ";
        }
        for (int i = 0; i < WIDTH; ++i) {
            std::cout << "\t" << std::setw(2) << i;
        }
        std::cout << "\n";

        for (int i = 0; i < HEIGHT; ++i) {
            std::cout << std::setw(2) << i;
            for (int j = 0; j < WIDTH; ++j) {
                if (area[i][j]) {
                    std::cout << "\t" << std::setw(2) << area[i][j]->value;
                } else {
                    std::cout << "\t" << "  ";
                }
            }
            std::cout << "\t" << std::setw(2) << i;
            std::cout << "\n";

        }

        std::cout << "  ";
        for (int i = 0; i < WIDTH; ++i) {
            std::cout << "\t" << std::setw(2) << i;
        }
        if (has_finish_) {
            std::cout << "\t" << " F";
        } else {
            std::cout << "\t" << "  ";
        }
        std::cout << "\n";
    }

};

class PlayerStrategy;

class Player {
public:
    Player() = delete;

    Player(unsigned int player_number, std::vector<std::unique_ptr<Card>> draw_pile,
           std::unique_ptr<PlayerStrategy> strategy) :
            player_number(player_number),
            draw(std::move(draw_pile)),
            strategy(std::move(strategy)) {

        draw_to_hand_size();
    }

    Player(Player &) = delete;

    void draw_to_hand_size() {
        while (hand.size() < 5 && !draw.empty()) {
            hand.push_back(std::move(draw.back()));
            draw.pop_back();
        }
    }

    std::vector<std::unique_ptr<Card>> draw;
    std::vector<std::unique_ptr<Card>> discard;
    std::vector<std::unique_ptr<Card>> hand;

    const unsigned int player_number;
    std::unique_ptr<PlayerStrategy> strategy;

};

//TODO is there really nothing in std that checks for duplicates and does not modify the container or require it being sorted?
template<class T>
bool hasDuplicates(const std::vector<T> &arr) {
    for (std::size_t i = 0; i < arr.size(); ++i) {
        for (std::size_t j = i + 1; j < arr.size(); ++j) {
            if (arr[i] == arr[j])
                return true;
        }
    }
    return false;
}


struct Turn {
    bool has_lost = true;
    std::vector<unsigned int> cards_to_discard;
    int card_to_play = -1;
    std::pair<int, int> position_played = {-1, -1};
    bool is_discard_phase = false;

    bool is_valid(const PlayArea &area, const std::vector<std::unique_ptr<Card>> &hand) {

        if (is_discard_phase) {
            if (hasDuplicates(cards_to_discard)) {
                return false;
            }
            for (auto c: cards_to_discard) {
                if (c >= hand.size()) { return false; }
            }
            // the Game Manager should check if the Agent honors the negotiation Result
            return true;
        }

        assert(not is_discard_phase);

        if (hand.empty()) {
            return has_lost;// must loose
        }

        if (has_lost) {
            // check if a different turn was possible
            if (hand.size() > 1) {
                // could discard
                return false;
            } else {
                // check if there is a large enough gap to play just one card
                for (int i = 0; i < PlayArea::WIDTH; ++i) {
                    for (int j = 0; j < PlayArea::HEIGHT; ++j) {
                        if (area.get_num_discard(std::make_pair(i, j), hand[0]->value) == 0) {
                            // could play at this position
                            return false;
                        }
                    }

                }
            }
        }


        //all cards to discard must be different
        if (hasDuplicates(cards_to_discard)) {
            return false;
        }

        // dont play just discard
        if (card_to_play == -1) {
            if (cards_to_discard.size() == 2) {
                return cards_to_discard[0] < hand.size() && cards_to_discard[1] < hand.size();
            } else { return false; }
        }

        if (card_to_play > hand.size()) {
            return false;
        }

        assert(hand[card_to_play] != nullptr);

        // if at least one start card in hand and not played so far
        if (area.has_start() && std::accumulate(hand.begin(), hand.end(), 0, [](auto accu, const auto &c) {
            if (c->value == Card::START) {
                return accu + 1;
            } else { return accu; }
        }) > 0) {
            return (hand[card_to_play]->value == Card::START &&
                    cards_to_discard.empty());
        }

        int num_discard = area.get_num_discard(position_played, hand[card_to_play]->value);
        if (num_discard == -1) {
            return false;
        }
        if (num_discard == cards_to_discard.size()) {
            for (auto c: cards_to_discard) {
                if (c >= hand.size() || c == card_to_play) { return false; }
            }
            return true;
        }

        return false;
    }

};

class PlayerStrategy {
public:
    explicit PlayerStrategy(unsigned int player_number) : player_number(player_number) {}

    PlayerStrategy(PlayerStrategy &) = delete;

    virtual void register_move(unsigned int player_number, Turn turn_made) = 0;

    virtual Turn make_turn(const GameManager &GM, const std::vector<std::unique_ptr<Card>> &hand) = 0;

    virtual int negotiate_discard_phase(const GameManager &GM, const std::vector<int> current_offer) = 0;

    virtual Turn perform_discard(const GameManager &GM, const std::vector<int> negotiation_result) = 0;

    const unsigned int player_number;
};

class GameManager {

public:
    GameManager() = delete;

    GameManager(GameManager &) = delete;

    PlayArea area = PlayArea();

    size_t get_hand_size(unsigned int player_number) {
        assert(player_number < players.size());
        return players[player_number]->hand.size();
    }

    size_t get_deck_size(unsigned int player_number) {
        assert(player_number < players.size());
        return players[player_number]->draw.size();
    }

    size_t get_discard_size(unsigned int player_number) {
        assert(player_number < players.size());
        return players[player_number]->discard.size();
    }

    size_t get_num_players() {
        return players.size();
    }

    template<class R>
    static bool RunNewGame(std::vector<std::unique_ptr<PlayerStrategy>> strategies, R &rng) {
        auto num_players = strategies.size();
        assert(MIN_PLAYER_COUNT <= num_players);
        assert(num_players <= MAX_PLAYER_COUNT);

        auto deck = create_deck();

        std::shuffle(std::begin(deck), std::end(deck), rng);

        std::vector<std::unique_ptr<Player>> players;

        unsigned int cards_per_player = deck.size() / num_players;
        auto reminder = deck.size() % num_players;

        for (int i = 0; i < num_players; ++i) {
            unsigned int cards_this_player = cards_per_player;
            if (reminder > 0) {
                cards_this_player++;
                reminder--;
            }
            std::vector<std::unique_ptr<Card>> this_players_deck;
            this_players_deck.reserve(cards_this_player + 1);

            this_players_deck.push_back(std::make_unique<Card>(Card::START));
            for (int j = 0; j < cards_this_player; ++j) {
                this_players_deck.push_back(std::move(deck.back()));
                deck.pop_back();
            }
            std::shuffle(std::begin(this_players_deck), std::end(this_players_deck), rng);

            assert(strategies[i]->player_number == i);
            players.push_back(std::make_unique<Player>(i, std::move(this_players_deck), std::move(strategies[i])));
        }
        auto GM = GameManager(std::move(players));
        return GM.run_game();
    }

private:
    explicit GameManager(std::vector<std::unique_ptr<Player>> players) : players(std::move(players)) {

    }

    std::vector<int> run_discard_phase_negotiation() {

        std::vector<int> current_offer(players.size(), -1);
        std::vector<int> new_offer(players.size(), -1);

        // operator + is default for accumulate
        while (std::accumulate(current_offer.begin(), current_offer.end(), 0) != NUM_DISCARD_DISCARD_PHASE) {
            for (auto &p: players) {
                new_offer[p->player_number] = p->strategy->negotiate_discard_phase(*this, current_offer);
            }
            current_offer = new_offer;
        }

    }

    // return if won
    bool run_game() {

        while (true) {
            for (auto &p: players) {
                auto turn = p->strategy->make_turn(*this, p->hand);
                assert(turn.is_valid(area, p->hand));
                if (turn.has_lost) {
                    return false;// lost
                }
                // track if start card was played
                bool enter_discard_phase = false;
                // execute turn
                if (turn.card_to_play != -1) {
                    enter_discard_phase = p->hand[turn.card_to_play]->value == Card::START;
                    area.play_card(turn.position_played, std::move(p->hand[turn.card_to_play]));
                    p->hand.erase(p->hand.begin() + turn.card_to_play);
                }
                for (auto d: turn.cards_to_discard) {
                    p->discard.push_back(std::move(p->hand[d]));
                    p->hand.erase(p->hand.begin() + d);
                }
                // register turn with other players, so they know what is going on
                for (auto &other_p: players) {
                    if (p != other_p) {// no need to register with itself
                        other_p->strategy->register_move(p->player_number, turn);
                    }
                }
                p->draw_to_hand_size();

                if (area.has_finish()) {
                    return true; // won
                }
                if (enter_discard_phase) {
                    // negotiation phase
                }
            }
        }
    }

    std::vector<std::unique_ptr<Player>> players;

};

class HumanPlayer : public PlayerStrategy {

public:
    explicit HumanPlayer(unsigned int player_number) : PlayerStrategy(player_number) {}

    void register_move(unsigned int player_number, Turn turn_made) override {};

    Turn make_turn(const GameManager &GM, const std::vector<std::unique_ptr<Card>> &hand) override {

        Turn turn_to_make;
        //TODO implement
        turn_to_make.has_lost = true;
        return turn_to_make;
    }

    int negotiate_discard_phase(const GameManager &GM, const std::vector<int> current_offer) override {
        assert(false);
        //TODO implement
    }

    Turn perform_discard(const GameManager &GM, const std::vector<int> negotiation_result) override {
        assert(false);
        //TODO implement
    }
};


int main() {

    //TODO specify RNG to use as parameter?
    auto rng = std::default_random_engine{};

    std::vector<std::unique_ptr<PlayerStrategy>> strategies;

    for (int i = 0; i < 3; ++i) {
        strategies.push_back(std::make_unique<HumanPlayer>(i));
    }

    bool game_won = GameManager::RunNewGame(std::move(strategies), rng);


    std::cout << "Game Won? " << game_won << "\n";


    return 0;

}
