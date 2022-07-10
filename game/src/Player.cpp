#include "Player.h"

Player::Player(unsigned id, Team team, std::weak_ptr<StarshipObject> ship):
    team(team), ship(ship), id(id){}
