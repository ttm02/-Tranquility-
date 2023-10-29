#include <iostream>

#include <utility>
#include <vector>
#include <memory>

#include <algorithm>
#include <random>
#include <iomanip>
#include <cassert>
#include <limits>

#include "GameManager.h"
#include "HumanPlayer.h"
#include "BinaryPartitionStrategy.h"



int main() {

    // TODO use a seed
    auto rng = std::default_random_engine{};

    std::vector<std::unique_ptr<PlayerAgent>> strategies;

    for (int i = 0; i < 2; ++i) {
        strategies.push_back(std::make_unique<BinaryPartitionStrategy>(i));
    }

    bool game_won = GameManager::RunNewGame(std::move(strategies), rng);


    std::cout << "Game Won? " << game_won << "\n";


    return 0;

}
