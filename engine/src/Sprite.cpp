#include "Sprite.h"
#include <glad.h>
#include <iostream>
#include <Renderer.h>
#include "Texture.h"
#include "Shader.h"
#include "Renderer.h"

glm::mat4 DynamicSprite::buildTransform(const glm::vec2& pos, float rotation, const glm::vec2& size) {
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(pos.x, pos.y, 0.0f));
    transform = glm::rotate(transform, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, glm::vec3(size, 1.0f));
    return transform;
}


BackgroundTile::BackgroundTile(const std::shared_ptr<Texture>& texture,
                               const std::shared_ptr<Shader>& shader,
                               const glm::vec2& size):
    texture(texture), shader(shader), size(size){}

void BackgroundTile::draw(Renderer& renderer) {
    texture->activate();
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::scale(transform, glm::vec3(Renderer::N_BG_REPEAT * size.x,
                                                Renderer::N_BG_REPEAT * size.y,
                                                1.0f));
    shader->setTransform(transform);
    constexpr auto color = glm::vec4(1, 1, 1, 1);
    shader->setColor(color);
    renderer.renderBackground();
}





