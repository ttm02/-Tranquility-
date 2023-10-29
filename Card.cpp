//
// Created by tim on 29.10.23.
//

#include "Card.h"

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
