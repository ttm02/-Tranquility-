#include <iostream>

#include <vector>
#include <memory>

#include <algorithm>
#include <random>
#include <iomanip>

class Card {
public:
    const unsigned int value;

    // max value is inclusive
    static const unsigned int MAX_VALUE = 80;

    // special value
    static const unsigned int START = 100;

    static const unsigned int FINISH = 101;


    Card(unsigned int v) : value(v) {};

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
        deck.push_back(std::make_unique<Card>(Card::START));//TODO each player gets one start card
        deck.push_back(std::make_unique<Card>(Card::FINISH));
    }
    for (unsigned int i = 0 + 1; i < Card::MAX_VALUE + 1; ++i) {
        deck.push_back(std::make_unique<Card>(i));
    }

    return deck;

};

class PlayArea {
public:
    PlayArea() {
        for (int i = 0; i < 6; ++i) {
            area.push_back(std::vector<std::unique_ptr<Card>>(6));
            // nullptr is default
        }
    }

    PlayArea(PlayArea &) = delete;

    std::vector<std::vector<std::unique_ptr<Card>>> area;


    bool has_start = false;
    bool has_finish = false;

    void print() const {
        // first row
        if (has_start) {
            std::cout << " S";
        } else {
            std::cout << "  ";
        }
        for (int i = 0; i < 6; ++i) {
            std::cout << "\t" << std::setw(2) << i;
        }
        std::cout << "\n";

        for (int i = 0; i < 6; ++i) {
            std::cout << std::setw(2) << i;
            for (int j = 0; j < 6; ++j) {
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
        for (int i = 0; i < 6; ++i) {
            std::cout << "\t" << std::setw(2) << i;
        }
        if (has_finish) {
            std::cout << "\t" << " F";
        } else {
            std::cout << "\t" << "  ";
        }
        std::cout << "\n";
    }

};

class Player {
public:
    Player() = delete;

    Player(std::vector<std::unique_ptr<Card>> draw_pile) {

        draw = std::move(draw_pile);
        draw_to_hand_size();
    }

    Player(Player &) = delete;

    void draw_to_hand_size() {
        while (hand.size() < 5 && draw.size() > 0) {
            hand.push_back(std::move(draw.back()));
            draw.pop_back();
        }
    }

    std::vector<std::unique_ptr<Card>> draw;
    std::vector<std::unique_ptr<Card>> discard;
    std::vector<std::unique_ptr<Card>> hand;

    //TODO  needs also a "Log" to know which other player has done what
    // do we need to get a reference to the other players?
    // e.g. to count the stack sizes?
    virtual bool make_turn(PlayArea &area) = delete;

};

int main() {
    std::cout << "Hello, World!" << std::endl;

    auto rng = std::default_random_engine{};

    auto deck = create_deck();

    std::shuffle(std::begin(deck), std::end(deck), rng);

    PlayArea area = PlayArea();

    area.area[2][3] = std::move(deck.back());
    deck.pop_back();
    area.area[2][4] = std::move(deck.back());
    deck.pop_back();
    area.area[3][3] = std::move(deck.back());
    deck.pop_back();

    area.print();

    return 0;

}
