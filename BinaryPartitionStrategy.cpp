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

    // idea find the position that is "as much middle as possible"
    // if possible: fill in gap if a card is safe to discard

    // when no "middle" gap exists anymore: play the cheapest card, discard 2 if it becomes too expansive

    auto middle_gap = find_best_middle_card_to_play(GM, hand);

    std::cout << "middle position: " << std::get<1>(middle_gap) << "Card To Play:"
              << hand[std::get<2>(middle_gap)]->value << "\n";

    assert(std::get<1>(middle_gap) != -1);//TODO implement

    turn.position_played = std::get<1>(middle_gap);
    turn.card_to_play = std::get<2>(middle_gap);


    // print before each turn to see what is happening
    GM.area.print();
    std::cout << "Confirm Turn ";
    std::cin.get();// wait for enter
    std::cout << "\n";

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

std::tuple<int, int, int> BinaryPartitionStrategy::find_best_middle_card_to_play(const GameManager &GM,
                                                                                 const std::vector<std::unique_ptr<Card>> &hand) {

    unsigned int previous_played_pos = 0;
    unsigned int current_best_pos = -1;
    unsigned int current_card_to_play = -1;
    unsigned int current_best_delta = std::numeric_limits<int>::max();

    for (int i = 0; i <= PlayArea::LENGTH; ++i) {
        if (GM.area.get_area()[i] != nullptr || i == PlayArea::LENGTH) {

            if (i - previous_played_pos > 3) {
                // otherwise: no space to play in between without adjacency

                unsigned current_pos = i;
                unsigned val_right = Card::MAX_VALUE;
                if (GM.area.get_area()[i] != nullptr) {
                    val_right = GM.area.get_area()[i]->value;
                }
                unsigned val_left = 1;
                if (GM.area.get_area()[previous_played_pos] != nullptr) {
                    val_left = GM.area.get_area()[previous_played_pos]->value;
                }
                // +1 for odd values (for even values ther is no exact middle, we also stick with +1)
                unsigned middle_pos = previous_played_pos + 1 + ((i - previous_played_pos) / 2);
                int middle_value =
                        val_left + (val_right - val_left) / 2;
                auto best_fit = find_best_card(middle_value, hand);
                if (current_best_delta > best_fit.first) {
                    current_best_delta = best_fit.first;
                    current_card_to_play = best_fit.second;
                    current_best_pos = middle_pos;
                }
            }
            previous_played_pos = i;
        }
    }

    return std::make_tuple(current_best_delta, current_best_pos, current_card_to_play);


}
