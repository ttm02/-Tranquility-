#ifndef TRANQUILITY_PLAYERAGENT_H
#define TRANQUILITY_PLAYERAGENT_H

#include <vector>
#include <memory>
#include "Card.h"

// #include "GameManager.h"
// is included below so that the game Manager can use it in templated functions without the error of incomplete class
class GameManager;

struct Turn;


class PlayerAgent {
public:
    explicit PlayerAgent(unsigned int player_number) : player_number(player_number) {}

    PlayerAgent(PlayerAgent &) = delete;

    virtual void register_move(unsigned int player_number, Turn turn_made) = 0;

    virtual Turn make_turn(const GameManager &GM, const std::vector<std::unique_ptr<Card>> &hand) = 0;

    virtual int negotiate_discard_phase(const GameManager &GM, const std::vector<int> current_offer) = 0;

    virtual Turn perform_discard(const GameManager &GM, const std::vector<int> negotiation_result) = 0;

    const unsigned int player_number;
};

#include "GameManager.h"

#endif //TRANQUILITY_PLAYERAGENT_H
