#ifndef ANIMATION_H
#define ANIMATION_H
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <string>
#include "Sprite.h"

class Renderer;
class Texture;
class Shader;

class Animation: public DynamicSprite {
public:

    Animation(std::shared_ptr<Shader> shader,
              std::vector<std::shared_ptr<Texture>> textures,
              const glm::vec2& size,
              const glm::vec2& pos,
              const glm::vec2& velocity,
              float ttl,
              float rotation=0);

    virtual ~Animation() = default;

    void update(float dt) override;
    void draw(Renderer& renderer) override;
    bool isAlive() const override;
    glm::vec2 getPos() const override;
    bool isPointInside(const glm::vec2& pos) const override;
    glm::vec2 getVelo() const override;
    bool isTrackable() const override;
protected:
    std::vector<std::shared_ptr<Texture>> textures;
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture> getCurrentTexture() const;
    glm::vec2 pos, size, velocity;
    float counter, ttl, rotation;
};


#endif // ANIMATION_H
