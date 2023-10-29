//
// Created by tim on 29.10.23.
//

#ifndef TRANQUILITY_PLAYER_H
#define TRANQUILITY_PLAYER_H

class PlayerAgent;

#include "PlayerAgent.h"

#include <algorithm>
#include <memory>

class Player {
public:
    Player() = delete;

    Player(unsigned int player_number, std::vector<std::unique_ptr<Card>> draw_pile,
           std::unique_ptr<PlayerAgent> strategy) :
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
    std::unique_ptr<PlayerAgent> strategy;

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


#endif //TRANQUILITY_PLAYER_H
