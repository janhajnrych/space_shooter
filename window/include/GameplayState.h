#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <string>
#include <memory>
#include <list>
#include <vector>
#include "Sprite.h"
#include "Player.h"

struct GameplayState {
    std::list<std::shared_ptr<DynamicSprite>> sprites;
    std::vector<std::shared_ptr<Player>> players;
};

#endif // GAMESTATE_H
