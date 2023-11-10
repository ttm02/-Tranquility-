//
// Created by tim on 29.10.23.
//

#include "BinaryPartitionStrategy.h"
#include "GameManager.h"
#include "HumanPlayer.h"

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


    //DEBUG:
    print_hand(hand);
    GM.area.print();

    // idea find the position that is "as much middle as possible"
    // if possible: fill in gaps if a card is safe to discard

    // when no "middle" gap exists anymore: play the cheapest card, discard 2 if it becomes too expansive

    auto middle_gap = find_best_middle_card_to_play(GM, hand);

    auto fill_gap = find_best_adjacent(GM, hand);

    std::cout << "Safe to discard: ";
    int num_safe_discards = 0;
    for (int i = 0; i < hand.size(); ++i) {
        if (is_card_safe_to_discard(GM, hand[i]->value)) {
            num_safe_discards++;
            std::cout << ", " << hand[i]->value;
        }
    }
    std::cout << "\n";

    if (num_safe_discards >= std::get<0>(fill_gap)) {
        assert(false && "TODO IMPLEMENT FILL GAP");
    }

    assert(std::get<1>(middle_gap) != -1);//TODO implement
    std::cout << "middle position: " << std::get<1>(middle_gap) << " Card To Play: "
              << hand[std::get<2>(middle_gap)]->value << "\n";


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


// binary search: best position for the given card inside of a larger gap
// start pos and end pos are inclusive
int find_pos_for_card(int card_value, int start_pos, int end_pos, int start_value, int end_value) {

    assert(start_pos <= end_pos);
    assert(start_value <= end_value);

    int length = end_pos - start_pos + 1;
    int middle_value = start_value + (end_value - start_value) / 2;
    int middle_pos = start_pos + length / 2;

    if (start_value == end_value) {
        return middle_pos;
    }

    if (length == 1) {
        assert(start_pos == end_pos);
        return start_pos;
    }
    if (length == 2) {
        return card_value < middle_value ? start_pos : end_pos;
    }

    int left_pos = find_pos_for_card(card_value, start_pos, middle_pos, start_value, middle_value);
    int right_pos = find_pos_for_card(card_value, middle_pos, end_pos, middle_value, end_value);
    // both times the middle is included
    // meaning that one time the middle pos will be "best" i.e. in the wrong array side
    // or both are middle
    return left_pos == middle_pos ? right_pos : left_pos;
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
                unsigned val_right = Card::MAX_VALUE + 1;
                if (GM.area.get_area()[i] != nullptr) {
                    val_right = GM.area.get_area()[i]->value;
                }
                unsigned val_left = 1 - 1;
                if (GM.area.get_area()[previous_played_pos] != nullptr) {
                    val_left = GM.area.get_area()[previous_played_pos]->value;
                }
                for (int j = 0; j < hand.size(); ++j) {
                    // positions -2 to leave space for the adjacend card (will be handled in different method)
                    // therefore we also subtract 2 from value search space
                    //but the endings needs to be included fully
                    int left_begin_search =
                            GM.area.get_area()[previous_played_pos] != nullptr ? previous_played_pos + 2 : 0;
                    int right_begin_search = GM.area.get_area()[current_pos] != nullptr ? current_pos - 2 : current_pos;

                    int best_pos = find_pos_for_card(hand[j]->value,
                                                     left_begin_search, right_begin_search,
                                                     val_left + 2, val_right - 2);
                    int val_diff = val_right - val_left;
                    int pos_diff = current_pos - previous_played_pos;
                    double value_per_step = ((double) val_diff) /
                                            ((double) pos_diff);
                    int approx_value_required = val_left + (int) (value_per_step *
                                                                  (double) (best_pos - previous_played_pos));
                    int delta = std::abs(hand[j]->value - approx_value_required);


                    if (current_best_delta > delta
                        && val_left < hand[j]->value &&
                        hand[j]->value < val_right
                            ) {
                        // check if this move invalidates the board
                        // i.e. not enough possible cards fo fill the resulting gaps
                        bool is_valid =
                                // left side
                                hand[j]->value - val_left >= best_pos - previous_played_pos &&
                                // right side
                                val_right - hand[j]->value >= current_pos - best_pos;
                        if (is_valid) {
                            current_best_delta = delta;
                            current_card_to_play = j;
                            current_best_pos = best_pos;
                        }
                    }
                }
            }
            previous_played_pos = i;
        }
    }

    std::cout << "Best Delta for middle Play: " << current_best_delta << "\n";

    return std::make_tuple(current_best_delta, current_best_pos, current_card_to_play);


}


std::tuple<int, int, int>
BinaryPartitionStrategy::find_best_adjacent(const GameManager &GM, const std::vector<std::unique_ptr<Card>> &hand) {


    unsigned int current_best_pos = -1;
    unsigned int current_card_to_play = -1;
    unsigned int current_best_num_discard = std::numeric_limits<int>::max();

    for (int i = 0; i < PlayArea::LENGTH; ++i) {
        for (int j = 0; j < hand.size(); ++j) {
            int new_num_discard = GM.area.get_num_discard(i, hand[j]->value);
            if (new_num_discard > 0 && new_num_discard < current_best_num_discard) {
                current_best_num_discard = new_num_discard;
                current_card_to_play = j;
                current_best_pos = i;
            }
        }

    }


    std::cout << "Best discard to adjacent: " << current_best_num_discard << "\n";
    return std::make_tuple(current_best_num_discard, current_best_pos, current_card_to_play);

}

bool BinaryPartitionStrategy::is_card_safe_to_discard(const GameManager &GM, int card_value) {
    // it is safe if:

    // start is not needed anymore
    if (card_value == Card::START && GM.area.has_start()) {
        return true;
    }
    //TODO finish?
    // is safe if we have multiple in hand or if we know we have a finish card left in draw:
    // if we havent discarded NUM_Finish/num_players yet

    int larger = -1;
    int smaller = -1;

    for (int i = 0; i < PlayArea::LENGTH; ++i) {
        if (GM.area.get_area()[i] != nullptr) {
            int value_in_field = GM.area.get_area()[i]->value;
            if (value_in_field < card_value) {
                smaller = i;
            }
            if (value_in_field > card_value) {
                larger = i;
                break;
            }
        }
    }

    //we know that the safety margin may increase when the negotiate discard phase happens
    // it may also be the case that it increases when other has only unsafe cards but must play
#define SAFTEY_MARGIN 1
    if (larger != -1 && smaller != -1) {
        if (larger - smaller == 1) {
            // no more gaps in between
            //immedeately adjacent
            return true;
        }
        if (larger - smaller == 2) {
            //only one space left
            return GM.area.get_area()[smaller]->value + SAFTEY_MARGIN < card_value &&
                   card_value > GM.area.get_area()[larger]->value - SAFTEY_MARGIN;
        }
    }

    if (smaller == -1 && larger != -1) {
        // so small it will not be played anymore
        if (larger == 0) {
            return true;
        }
        if (larger == 1) {
            return card_value > GM.area.get_area()[larger]->value - SAFTEY_MARGIN;
        }
    }

    if (smaller != -1 && larger == -1) {
        // so large it will not be played anymore
        if (smaller == PlayArea::LENGTH - 1) {
            return true;
        }
        if (smaller == PlayArea::LENGTH - 2) {
            return card_value < GM.area.get_area()[smaller]->value + SAFTEY_MARGIN;
        }
    }

    return false;

}