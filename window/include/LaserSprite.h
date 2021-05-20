#ifndef LASERSPRITE_H
#define LASERSPRITE_H
#include "Sprite.h"

class SpriteGroup;

class LaserSprite: public DynamicSprite
{
public:
    LaserSprite(std::shared_ptr<Shader> shader,
                std::shared_ptr<Texture> texture,
                glm::vec2 size,
                const glm::vec4& color,
                const glm::vec2& pos,
                const glm::vec2& velocity,
                float ttl);
    LaserSprite(std::shared_ptr<Shader> shader,
                std::shared_ptr<Texture> texture,
                glm::vec2 size,
                const glm::vec4& color);
    void activate(glm::vec2 pos, glm::vec2 velo, float ttl);
    void draw(Renderer& renderer) override;
    void update(float dt) override;
    bool isAlive() const override;
    bool isPointInside(const glm::vec2& pos) const override;
    glm::vec2 getPos() const override;
    glm::vec2 getVelo() const override;
    void die();
private:
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture> texture;
    glm::vec4 color;
    glm::vec2 size, pos, velocity;
    float rotation, timeToLive;
};


#endif // LASERSPRITE_H
