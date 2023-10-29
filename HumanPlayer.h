//
// Created by tim on 29.10.23.
//

#ifndef TRANQUILITY_HUMANPLAYER_H
#define TRANQUILITY_HUMANPLAYER_H

#include "PlayerAgent.h"
#include "GameManager.h"

#include <cassert>

class HumanPlayer : public PlayerAgent {

public:
    explicit HumanPlayer(unsigned int player_number) : PlayerAgent(player_number) {}

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


#endif //TRANQUILITY_HUMANPLAYER_H
