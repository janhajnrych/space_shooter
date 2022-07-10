#ifndef PLAYER_H
#define PLAYER_H
#include <memory>
#include "Starship.h"
#include "Team.h"

struct Player {
  Team team;
  std::weak_ptr<StarshipObject> ship;
  unsigned id;
  Player(unsigned id, Team team, std::weak_ptr<StarshipObject> ship);
};


#endif // PLAYER_H
