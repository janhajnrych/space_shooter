#include "Animation.h"
#include <glad.h>
#include <iostream>
#include <Renderer.h>
#include "Texture.h"
#include "Shader.h"
#include "Renderer.h"


Animation::Animation(std::shared_ptr<Shader> shader,
                     std::vector<std::shared_ptr<Texture>> textures,
                     const glm::vec2& size,
                     const glm::vec2& pos,
                     const glm::vec2& velocity,
                     float ttl, float rotation):
    textures(textures), shader(shader), pos(pos), size(size), velocity(velocity), counter(0), ttl(ttl), rotation(rotation){}

std::shared_ptr<Texture> Animation::getCurrentTexture() const {
    std::size_t index = static_cast<size_t>(counter/ttl * textures.size());
    return textures.at(index % textures.size());
}

void Animation::update(float dt) {
    if (counter < ttl)
        counter+=dt;
}

void Animation::draw(Renderer& renderer) {
    getCurrentTexture()->activate();
    shader->setTransform(buildTransform(pos, rotation, size));
    constexpr glm::vec4 color(1, 1, 1, 1);
    shader->setColor(color);
    renderer.renderSprites();
}

bool Animation::isAlive() const {
    return (counter <= ttl);
}

bool Animation::isPointInside(const glm::vec2& pos) const {
    return false;
}

glm::vec2 Animation::getPos() const {
    return pos;
}

glm::vec2 Animation::getVelo() const {
    return velocity;
}

bool Animation::isTrackable() const {
    return false;
}


