//
// Created by tim on 29.10.23.
//

#include <iomanip>
#include <iostream>
#include <cassert>
#include <numeric>

#include "PlayArea.h"


const unsigned int PlayArea::WIDTH;
const unsigned int PlayArea::HEIGHT;
const unsigned int  PlayArea::LENGTH;

void PlayArea::print() const {
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
            if (area[i * WIDTH + j]) {
                std::cout << "\t" << std::setw(2) << area[i * WIDTH + j]->value;
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

int PlayArea::get_num_discard(int pos, unsigned int new_card_value) const {
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
        auto num_gaps = std::accumulate(area.begin(), area.end(), 0, [](auto accu, const auto &card) {
            if (card == nullptr) {
                return accu + 1;
            } else {
                return accu + 0;
            }
        });
        if (not has_finish_ && num_gaps == 0 && has_start_) {
            return 0;
        } else {
            return -1;
        }
    }

    // position is in Boundaries and free
    if (pos > 0 && pos < LENGTH &&
        area[pos] == nullptr) {

        int left_neighbor = pos == 0 ? pos : pos + 1;
        int right_neighbor = pos == LENGTH - 1 ? pos : pos - 1;

        if (area[right_neighbor] == nullptr &&
            area[left_neighbor] == nullptr) {
            // no neighbor
            return 0;
        } else {
            unsigned int num_discard = std::numeric_limits<int>::max();

            if (area[right_neighbor] != nullptr) {
                if (area[right_neighbor]->value < new_card_value) {
                    return -1;
                }
                num_discard = std::min(num_discard,
                                       area[right_neighbor]->value - new_card_value);
            }
            if (area[left_neighbor] != nullptr) {
                if (new_card_value < area[left_neighbor]->value) {
                    return -1;
                }
                num_discard = std::min(new_card_value - num_discard,
                                       area[left_neighbor]->value);
            }
            assert(num_discard < std::numeric_limits<int>::max());
            return static_cast<int>(num_discard);
        }

    } else {
        return -1; // invalid
    }
}

void PlayArea::play_card(int pos, std::unique_ptr<Card> card) {

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
    area[pos] = std::move(card);
}