#include "Laser.h"
#include <iostream>

namespace  {
    static constexpr glm::vec2 size{0.125f, 0.005f};
}

LaserObject::LaserObject(Team team,
             const glm::vec2& pos,
             const glm::vec2& velocity,
             float ttl):
    TeamObject(team),
    pos(pos), velocity(velocity),
    rotation(glm::atan(velocity.y, velocity.x)), timeToLive(ttl){}

void LaserObject::update(float dt) {
    pos+= velocity * dt;
    timeToLive-= dt;
}

bool LaserObject::isAlive() const {
    return timeToLive > 0.0f;
}

bool LaserObject::isPointInside(const glm::vec2&) const {
    return false;
}

glm::vec2 LaserObject::getPos() const {
    return pos;
};

glm::vec2 LaserObject::getVelo() const {
    return velocity;
}

void LaserObject::die() {
    timeToLive = 0;
}

void LaserObject::hit() {
    die();
}

bool LaserObject::isTrackable() const {
    return false;
}

ObjectType LaserObject::getType() const {
    return ObjectType::Laser;
}

float LaserObject::getRotation() const {
    return rotation;
}

glm::vec2 LaserObject::getSize() const {
    return size;
}


