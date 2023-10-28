#include <iostream>

#include <vector>
#include <memory>

#include <algorithm>
#include <random>

class Card{
public:
    const unsigned int value;

    // max value is inclusive
    static const unsigned int MAX_VALUE=80;

    // special value
    static const unsigned int START=100;

    static const unsigned int FINISH=101;



    Card(unsigned int v) : value(v) {};
    // not copy able
    Card(const Card&) = delete;

    // default destructor

};

const unsigned int Card::MAX_VALUE;
const unsigned int Card::START;
const unsigned int Card::FINISH;


std::vector<std::unique_ptr<Card>> create_deck(){
        std::vector<std::unique_ptr<Card>> deck;

        deck.reserve(100);
    for (int i = 0; i < 5; ++i) {
        deck.push_back(std::make_unique<Card>(Card::START));
        deck.push_back(std::make_unique<Card>(Card::FINISH));
    }
    for (unsigned int i = 0+1; i < Card::MAX_VALUE+1; ++i) {
        deck.push_back(std::make_unique<Card>(i));
    }

        return deck;

};

int main() {
    std::cout << "Hello, World!" << std::endl;

    auto rng = std::default_random_engine {};

    auto deck=create_deck();

    std::shuffle(std::begin(deck), std::end(deck), rng);


    return 0;

}
