#ifndef PLAYER_H
#define PLAYER_H
#include <memory>

class StarshipSprite;

struct Player {
    std::weak_ptr<StarshipSprite> sprite;
    unsigned id;
    unsigned score = 0;
};

#endif // PLAYER_H
