#ifndef TRANQUILITY_PLAYAREA_H
#define TRANQUILITY_PLAYAREA_H

#include <vector>

#include "Card.h"

//TODO refactor; the play area is not 2d it is just presented as 2d
class PlayArea {
public:
    PlayArea() {
        for (int i = 0; i < 6; ++i) {
            area.push_back(std::vector<std::unique_ptr<Card>>(6));
            // nullptr is default
        }
    }

    PlayArea(PlayArea &) = delete;

private:
    std::vector<std::vector<std::unique_ptr<Card>>> area;
public:
    const std::vector<std::vector<std::unique_ptr<Card>>> &get_area() const { return area; };

    void play_card(std::pair<int, int> pos, std::unique_ptr<Card> card);

    // return -1 if move is invalid
    int get_num_discard(std::pair<int, int> pos, unsigned int new_card_value) const;

private:
    bool has_start_ = false;
    bool has_finish_ = false;
public:
    bool has_start() const { return has_start_; }

    bool has_finish() const { return has_finish_; }

    static const unsigned int WIDTH = 6;
    static const unsigned int HEIGHT = 6;

    void print() const;

};


#endif //TRANQUILITY_PLAYAREA_H
