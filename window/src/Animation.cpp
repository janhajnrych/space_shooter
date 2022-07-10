#include "Animation.h"
#include <glad.h>
#include <iostream>
#include <Renderer.h>
#include "Texture.h"
#include "Shader.h"
#include "Renderer.h"


Animation::Animation(std::shared_ptr<Shader> shader,
                     std::vector<std::shared_ptr<Texture>> textures,
                     std::shared_ptr<EvolvingObject> object):
    textures(textures), shader(shader), object(object) {}

Animation::~Animation(){}

std::shared_ptr<Texture> Animation::getCurrentTexture() const {
    std::size_t index = static_cast<size_t>(object->getProgress() * textures.size());
    return textures.at(index % textures.size());
}

void Animation::draw(Renderer& renderer) {
    getCurrentTexture()->activate();
    shader->setTransform(buildTransform(object->getPos(), object->getRotation(), object->getSize()));
    constexpr glm::vec4 color(1, 1, 1, 1);
    shader->setColor(color);
    renderer.renderSprites();
}

std::shared_ptr<DynamicObject> Animation::getObject() const {
    return object;
}


