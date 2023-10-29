//
// Created by tim on 29.10.23.
//

#include "BinaryPartitionStrategy.h"
#include "GameManager.h"

#include <iostream>

Turn BinaryPartitionStrategy::make_turn(const GameManager &GM, const std::vector<std::unique_ptr<Card>> &hand) {

    //TODO extract this as utility to base class
    Turn turn;
    turn.has_lost = true;
    if (turn.is_valid(GM.area, hand)) {
        std::cout << "HAS LOST\n";
        return turn;
    }
    turn.has_lost = false;

    if (not GM.area.has_start() && num_start_in_hand(hand) > 1) {
        for (int i = 0; i < hand.size(); ++i) {
            if (hand[i]->value == Card::START) {
                turn.card_to_play = i;
                return turn;
            }

        }
    }

    if (GM.area.get_num_gaps() == 0) {
        for (int i = 0; i < hand.size(); ++i) {
            if (hand[i]->value == Card::FINISH) {
                turn.card_to_play = i;
                return turn;
            }
        }
    }
    // END Check for lost, start, finish

    return turn;
}


int
BinaryPartitionStrategy::negotiate_discard_phase(const GameManager &GM, const std::vector<std::unique_ptr<Card>> &hand,
                                                 const std::vector<int> current_offer) {

    //TODO implement
    assert(false);
}

Turn BinaryPartitionStrategy::perform_discard(const GameManager &GM, const std::vector<std::unique_ptr<Card>> &hand,
                                              const std::vector<int> negotiation_result) {

    //TODO implement
    assert(false);
}
