//
// Created by tim on 29.10.23.
//

#include "HumanPlayer.h"

#include <iomanip>
#include <iostream>

void print_hand(const std::vector<std::unique_ptr<Card>> &hand,
                             std::vector<int> to_exclude) {
    std::cout << "Your Hand:\n";

    assert(hand.size() <= 10);
    for (int i = 0; i < hand.size(); ++i) {
        std::cout << "(" << i << ")\t";
    }
    std::cout << "\n";

    for (int i = 0; i < hand.size(); ++i) {
        if (std::find(to_exclude.begin(), to_exclude.end(), i) == to_exclude.end()) {
            switch (hand[i]->value) {
                case Card::START:
                    std::cout << "  S\t";
                    break;
                case Card::FINISH:
                    std::cout << "  F\t";
                    break;
                default:
                    std::cout << std::setw(3) << hand[i]->value << "\t";
            }
        } else {
            std::cout << "   \t";
        }
    }
    std::cout << "\n";
}

int HumanPlayer::negotiate_discard_phase(
        const GameManager &GM, const std::vector<std::unique_ptr<Card>> &hand,
        const std::vector<int> current_offer) {
    std::cout << "Player " << player_number << "\n";
    std::cout << "Board:\n";
    GM.area.print();

    std::cout << "What the others are willing to discard?\n";

    for (int i: current_offer) {
        std::cout << i << "\t";
    }

    std::cout << "\n";

    print_hand(hand);
    int offer;
    std::cout << "Number of cards you are willing to discard ? ";
    std::cin >> offer;
    std::cout << "\n";

    return offer;
}

Turn HumanPlayer::perform_discard(
        const GameManager &GM, const std::vector<std::unique_ptr<Card>> &hand,
        const std::vector<int> negotiation_result) {

    std::cout << "Player " << player_number << "\n";
    std::cout << "Board:\n";
    GM.area.print();

    int num_cards_to_discard = negotiation_result[player_number];

    std::cout << "Players Agreed: you need to discard " << num_cards_to_discard << " Cards\n";


    Turn turn;
    turn.has_lost = false;
    turn.is_discard_phase = true;
    turn.cards_to_discard.push_back(-1);// invalidate to enter first loop iteration


    while (not turn.is_valid(GM.area, hand)) {
        std::vector<int> cards_to_exclude_showing = {};
        turn.cards_to_discard = {};
        cards_to_exclude_showing.push_back(turn.card_to_play);
        for (int i = 0; i < num_cards_to_discard; ++i) {
            print_hand(hand, cards_to_exclude_showing);
            std::cout << "Which cards to discard? (" << num_cards_to_discard - i << " remaining): ";
            int to_discard;
            std::cin >> to_discard;
            turn.cards_to_discard.push_back(to_discard);
            cards_to_exclude_showing.push_back(to_discard);
            std::cout << "\n";
        }
        if (not turn.is_valid(GM.area, hand)) {
            std::cout << "Invalid selection\n";
        }
    }
    return turn;

}

Turn HumanPlayer::make_turn(const GameManager &GM,
                            const std::vector<std::unique_ptr<Card>> &hand) {

    std::cout << "Player " << player_number << "\n";

    Turn turn;
    turn.has_lost = true;
    if (turn.is_valid(GM.area, hand)) {
        std::cout << "HAS LOST\n";
        return turn;
    }
    turn.has_lost = false;

    bool is_turn_valid = false;
    while (not turn.is_valid(GM.area, hand)) {
        std::cout << "Board:\n";
        GM.area.print();

        print_hand(hand);
        std::cout << "Which card to play? (-1: only discard 2)\n";
        std::cin >> turn.card_to_play;

        if (turn.card_to_play < -1 || turn.card_to_play >= hand.size()) {
            std::cout << "Choose a valid card!\n";
            continue;
        }

        if (turn.card_to_play != -1 &&
            (hand[turn.card_to_play]->value == Card::START ||
             hand[turn.card_to_play]->value == Card::FINISH)) {
            continue; // will check ofr validity and perform the turn or restart
        }

        int num_cards_to_discard = -1;

        if (turn.card_to_play != -1) {
            std::cout << "Which ROW to play into? ";
            int row = -1;
            int col = -1;
            std::cin >> row;
            std::cout << "\n Which COLUMN to play into? ";
            std::cin >> col;
            std::cout << "\n";
            if (row < 0 || row >= PlayArea::HEIGHT || col < 0 ||
                col >= PlayArea::WIDTH) {
                std::cout << "Choose a valid position!\n";
                continue;
            }
            turn.position_played = row * PlayArea::WIDTH + col;

            num_cards_to_discard = GM.area.get_num_discard(
                    turn.position_played, hand[turn.card_to_play]->value);

        } else {
            num_cards_to_discard = 2;
        }

        if (num_cards_to_discard == -1 || num_cards_to_discard > hand.size() ||
            (turn.card_to_play != -1 && num_cards_to_discard + 1 > hand.size())) {
            std::cout << "Number of cards to discard too large or position invalid\n";
            continue;
        }

        std::vector<int> cards_to_exclude_showing = {};
        turn.cards_to_discard = {};
        cards_to_exclude_showing.push_back(turn.card_to_play);
        for (int i = 0; i < num_cards_to_discard; ++i) {
            print_hand(hand, cards_to_exclude_showing);
            std::cout << "Which cards to discard? (" << num_cards_to_discard - i << " remaining): ";
            int to_discard;
            std::cin >> to_discard;
            turn.cards_to_discard.push_back(to_discard);
            cards_to_exclude_showing.push_back(to_discard);
            std::cout << "\n";
        }
        if (not turn.is_valid(GM.area, hand)) {
            std::cout << "Invalid selection\n";
        }
    }

    return turn;
}
