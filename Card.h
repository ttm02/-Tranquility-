#ifndef TRANQUILITY_CARD_H
#define TRANQUILITY_CARD_H


#include <vector>
#include <memory>

class Card {
public:
    const int value;

    // max value is inclusive
    static const int MAX_VALUE = 80;

    // special value
    static const int START = 100;

    static const int FINISH = 101;

    explicit Card(int v) : value(v) {};

    // not copy able
    Card(const Card &) = delete;

    // default destructor
};

std::vector<std::unique_ptr<Card>> create_deck();

#endif //TRANQUILITY_CARD_H
