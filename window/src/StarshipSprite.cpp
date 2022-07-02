#include "StarshipSprite.h"
#include <glad.h>
#include <iostream>
#include <sstream>
#include <random>
#include <Shader.h>
#include <Texture.h>
#include <Renderer.h>
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

StarshipSprite::StarshipSprite(std::shared_ptr<Shader> shader,
                               std::shared_ptr<Texture> texture,
                               glm::vec2 size,
                               const glm::vec2& pos, float rotation):
    shader(shader), texture(texture), size(size), pos(pos), rotation(rotation),
    velocity({0, 0}), thrust({0, 0}), angularSpeed(0), torque(0), fireCounter(0.0f), fireRate(0.0f), life(100){}

void StarshipSprite::update(float dt) {
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
            projectileEmitter.emit(createProjectile(static_cast<float>(M_PI/2.0)));
            projectileEmitter.emit(createProjectile(static_cast<float>(-M_PI/2.0)));
            fireCounter=1.0f;
        }
    }
    wrapPos(pos);
}

void StarshipSprite::setSteeringTorque(float torque) {
    this->torque = glm::radians(torque);
}

void StarshipSprite::setLinearThrust(float thrust) {
    thrust = std::min(maxThrust, thrust);
    this->thrust = glm::vec2(thrust*glm::cos(rotation),
                             thrust*glm::sin(rotation));
}

void StarshipSprite::setFireRate(float value) {
    fireRate = value;
}

bool StarshipSprite::isAlive() const {
    return life > 0;
}

void StarshipSprite::die() {
    life = 0;
}

void StarshipSprite::hit() {
    life--;
}

std::string StarshipSprite::serialize() const {
    std::ostringstream stream;
    stream << life << " " << int(pos.x * 100.0f) << " " << int(pos.y* 100.0f)
           << " " << int(velocity.x * 100.0f/maxSpeed) << " " << int(velocity.y* 100.0f/maxSpeed) << ";";
    return stream.str();
}

FireEvent StarshipSprite::createProjectile(float angle) const {
    FireEvent proj;
    proj.pos = pos -  size.x * glm::vec2(glm::cos(rotation + angle), glm::sin(rotation + angle));
    proj.pos+= 1.5f*size.y * glm::vec2(glm::cos(rotation), glm::sin(rotation));
    constexpr float range = 20.0f;
    constexpr float veloMag = 3.0f;
    proj.velo = velocity + radialToCart(veloMag, rotation);
    proj.ttl = range/veloMag;
    return proj;
}

Observable<FireEvent>& StarshipSprite::getWeapon() {
    return projectileEmitter;
}

void StarshipSprite::draw(Renderer& renderer) {
    texture->activate();
    shader->setTransform(buildTransform(pos, rotation, size));
    constexpr glm::vec4 color(1, 1, 1, 1);
    shader->setColor(color);
    renderer.renderSprites();
}

bool StarshipSprite::isPointInside(const glm::vec2& pos) const {
    return 0.9f*std::hypot(this->pos.x - pos.x, this->pos.y - pos.y) < std::hypot(size.x, size.y);
}

glm::vec2 StarshipSprite::getPos() const {
    return pos;
}

glm::vec2 StarshipSprite::getVelo() const {
    return velocity;
}

bool StarshipSprite::isTrackable() const {
    return true;
}


