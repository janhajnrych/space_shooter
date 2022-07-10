#include "ObjectSprite.h"
#include <glad.h>
#include <iostream>
#include "Texture.h"
#include "Shader.h"
#include "Renderer.h"

ColoredObjectSprite::ColoredObjectSprite(std::shared_ptr<Shader> shader,
                                         std::shared_ptr<Texture> texture,
                                         std::shared_ptr<DynamicObject> object,
                                         const glm::vec4& color):
    shader(shader), texture(texture), object(object), color(color){}

void ColoredObjectSprite::draw(Renderer& renderer) {
    texture->activate();
    shader->setTransform(buildTransform(object->getPos(), object->getRotation(), object->getSize()));
    shader->setColor(color);
    renderer.renderSprites();
}

std::shared_ptr<DynamicObject> ColoredObjectSprite::getObject() const {
    return object;
}



