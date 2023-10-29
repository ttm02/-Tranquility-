//
// Created by tim on 29.10.23.
//

#ifndef TRANQUILITY_BINARYPARTITIONSTRATEGY_H
#define TRANQUILITY_BINARYPARTITIONSTRATEGY_H

#include "PlayerAgent.h"
#include "GameManager.h"

class BinaryPartitionStrategy : public PlayerAgent {

public:
    explicit BinaryPartitionStrategy(unsigned int player_number) : PlayerAgent(player_number) {}

    void register_move(unsigned int player_number, Turn turn_made) override {};

    Turn make_turn(const GameManager &GM, const std::vector<std::unique_ptr<Card>> &hand) override;

    int negotiate_discard_phase(const GameManager &GM, const std::vector<std::unique_ptr<Card>> &hand,
                                const std::vector<int> current_offer) override;

    Turn perform_discard(const GameManager &GM, const std::vector<std::unique_ptr<Card>> &hand,
                         const std::vector<int> negotiation_result) override;


};


#endif //TRANQUILITY_BINARYPARTITIONSTRATEGY_H
