#include "LaserSprite.h"
#include <glad.h>
#include <iostream>
#include "Texture.h"
#include "Shader.h"
#include "Renderer.h"

LaserSprite::LaserSprite(std::shared_ptr<Shader> shader,
                         std::shared_ptr<Texture> texture,
                         glm::vec2 size,
                         const glm::vec4& color,
                         const glm::vec2& pos,
                         const glm::vec2& velocity,
                         float ttl):
    shader(shader), texture(texture),
    color(color), size(size), pos(pos),
    velocity(velocity), rotation(glm::atan(velocity.y, velocity.x)), timeToLive(ttl){}

LaserSprite::LaserSprite(std::shared_ptr<Shader> shader,
                         std::shared_ptr<Texture> texture,
                         glm::vec2 size,
                         const glm::vec4& color):
LaserSprite(shader, texture, size, color, glm::vec2(0, 0), glm::vec2(0, 0), 0){}

void LaserSprite::activate(glm::vec2 pos, glm::vec2 velo, float ttl) {
    this->pos = pos;
    this->velocity = velo;
    this->timeToLive = ttl;
    rotation = glm::atan(velocity.y, velocity.x);
}

void LaserSprite::update(float dt) {
    pos+= velocity * dt;
    timeToLive-= dt;
}

bool LaserSprite::isAlive() const {
    return timeToLive > 0.0f;
}

void LaserSprite::draw(Renderer& renderer) {
    texture->activate();
    shader->setTransform(buildTransform(pos, rotation, size));
    shader->setColor(color);
    renderer.renderSprites();
}

bool LaserSprite::isPointInside(const glm::vec2& pos) const {
    return false;
}

glm::vec2 LaserSprite::getPos() const {
    return pos;
};

glm::vec2 LaserSprite::getVelo() const {
    return velocity;
}

void LaserSprite::die() {
    timeToLive = 0;
}


