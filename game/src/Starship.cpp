#include "Starship.h"
#include <iostream>
#include <sstream>
#include <random>
#include <cmath>

namespace  {

void wrapFloat(float& value) {
    if (value > 1.0f)
       value-= 2.0f;
    if (value < -1.0f)
       value+= 2.0f;
}

void wrapPos(glm::vec2& pos) {
    wrapFloat(pos.x);
    wrapFloat(pos.y);
}

inline glm::vec2 radialToCart(float mag, float angle){
    return glm::vec2(mag*glm::cos(angle),
                     mag*glm::sin(angle));
}

}

GunObject::GunObject(float angle, DynamicObject& parent): angle(angle), parent(parent) {}

void GunObject::fire() const {
    if (!emitter.has_value())
        return;
    auto rotation = parent.getRotation();
    auto size = parent.getSize();
    auto pos = parent.getPos() -  size.x * glm::vec2(glm::cos(rotation + angle), glm::sin(rotation + angle));
    pos+= 1.5f* size.y * glm::vec2(glm::cos(rotation), glm::sin(rotation));
    auto velo = parent.getVelo() + radialToCart(3.0f, rotation);
    emitter.value()(pos, velo);
}

void GunObject::subscribe(Emitter emitter) {
    this->emitter = emitter;
}

StarshipObject::StarshipObject(Team team, const glm::vec2& size, const glm::vec2& pos, float rotation):
    TeamObject(team),
    size(size), pos(pos),
    velocity({0, 0}), thrust({0, 0}),
    rotation(rotation), angularSpeed(0), torque(0),
    fireCounter(0.0f), fireRate(0.0f), life(100) {
    guns.push_back(std::make_unique<GunObject>(static_cast<float>(M_PI/2.0), *this));
    guns.push_back(std::make_unique<GunObject>(static_cast<float>(-M_PI/2.0), *this));
}

StarshipObject::StarshipObject(Team team, size_t playerId):
    StarshipObject(team, defaultSize, getDefaultStartPos(playerId), 0.f){}

StarshipObject::~StarshipObject() {}

void StarshipObject::update(float dt) {
    constexpr float mass = 16.0f;
    constexpr float friction = maxThrust / (mass * maxSpeed * maxSpeed);
    auto veloMag = glm::length(velocity);  // simplified modelling
    const auto drag = friction * veloMag * velocity;
    velocity+= thrust * dt/mass - drag;

    veloMag = glm::length(velocity);
    velocity = radialToCart(veloMag, rotation);
    pos+= velocity * dt;
    angularSpeed = (veloMag/maxSpeed) * torque;
    rotation+= angularSpeed * dt;
    if (fireRate > 0) {
        if (fireCounter > 0){
            fireCounter-= dt * fireRate;
        } else {
            for (auto& gun : guns){
                gun->fire();
            }
            fireCounter=1.0f;
        }
    }
    wrapPos(pos);
}

void StarshipObject::jumpTo(const glm::vec2& pos) {
    this->pos = pos;
}

void StarshipObject::setVelo(const glm::vec2& velo) {
    this->velocity = velo;
}

void StarshipObject::setRot(float rot) {
    this->rotation = rot;
}

void StarshipObject::setSteeringTorque(float torque) {
    this->torque = glm::radians(torque);
}

void StarshipObject::setLinearThrust(float thrust) {
    thrust = std::min(4.f, thrust);
    this->thrust = glm::vec2(thrust * glm::cos(rotation),
                             thrust * glm::sin(rotation));
}

void StarshipObject::setFireRate(float value) {
    fireRate = value;
}

float StarshipObject::getFireRate() const {
    return fireRate;
}

bool StarshipObject::isAlive() const {
    return life > 0;
}

void StarshipObject::die() {
    life = 0;
}

void StarshipObject::hit() {
    life--;
}

bool StarshipObject::isPointInside(const glm::vec2& pos) const {
    return 0.9f*std::hypot(this->pos.x - pos.x, this->pos.y - pos.y) < std::hypot(size.x, size.y);
}

glm::vec2 StarshipObject::getPos() const {
    return pos;
}

glm::vec2 StarshipObject::getVelo() const {
    return velocity;
}

float StarshipObject::getRotation() const {
    return rotation;
}

glm::vec2 StarshipObject::getSize() const {
    return size;
}

bool StarshipObject::isTrackable() const {
    return true;
}

ObjectType StarshipObject::getType() const {
    return ObjectType::Ship;
}

glm::vec2 StarshipObject::getDefaultStartPos(size_t id) {
    float side = -2.f * float(id % 2) + 1.f;
    return glm::vec2(0.45f * side, 0.45f * side);
}

const std::vector<std::unique_ptr<GunObject>>& StarshipObject::getGuns() const {
    return guns;
}




