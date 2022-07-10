#ifndef GAMESTATE_H
#define GAMESTATE_H
#include "Player.h"
#include <unordered_map>

struct GameState {
    std::unordered_map<size_t, std::shared_ptr<Player>> players;
    virtual ~GameState();
    virtual void addShip(std::shared_ptr<StarshipObject> shipObject) =0;
};


#endif // GAMESTATE_H
