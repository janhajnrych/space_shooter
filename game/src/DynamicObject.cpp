#include "DynamicObject.h"
#include <iostream>

TeamObject::TeamObject(Team team): team(team){}

Team TeamObject::getTeam() const {
    return team;
}

TeamObject::~TeamObject(){}
