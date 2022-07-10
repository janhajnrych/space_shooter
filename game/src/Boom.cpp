#include "Boom.h"
#include <iostream>

BoomObject::BoomObject(const glm::vec2& size,
           const glm::vec2& pos,
           const glm::vec2& velocity,
           float ttl):
    size(size), pos(pos),velocity(velocity),
    rotation(glm::atan(velocity.y, velocity.x)), timeToLive(ttl), liefspan(ttl){}

BoomObject::~BoomObject(){}

void BoomObject::update(float dt) {
    pos+= velocity * dt;
    timeToLive-= dt;
}

bool BoomObject::isAlive() const {
    return timeToLive > 0.0f;
}

bool BoomObject::isPointInside(const glm::vec2&) const {
    return false;
}

glm::vec2 BoomObject::getPos() const {
    return pos;
};

glm::vec2 BoomObject::getVelo() const {
    return velocity;
}

float BoomObject::getRotation() const {
    return 0.f;
}

glm::vec2 BoomObject::getSize() const {
    return size;
}

void BoomObject::die() {
    timeToLive = 0;
}

void BoomObject::hit() {}

bool BoomObject::isTrackable() const {
    return false;
}

ObjectType BoomObject::getType() const {
    return ObjectType::Boom;
}

float BoomObject::getProgress() const {
    return 1.f - timeToLive/liefspan;
}


