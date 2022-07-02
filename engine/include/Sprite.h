#ifndef SPRITE_H
#define SPRITE_H
#include <glm/glm.hpp>
#include <memory>

class Renderer;
class Texture;
class Shader;

class Sprite
{
public:
    virtual ~Sprite() = default;
    virtual void draw(Renderer& renderer) =0;
};

class DynamicSprite: public Sprite {
public:
    static glm::mat4 buildTransform(const glm::vec2& pos, float rotation, const glm::vec2& size);
    virtual glm::vec2 getPos() const =0;
    virtual glm::vec2 getVelo() const =0;
    virtual ~DynamicSprite() = default;
    virtual void update(float dt) =0;
    virtual bool isAlive() const =0;
    virtual bool isPointInside(const glm::vec2& pos) const =0;
    virtual bool isTrackable() const =0;
};

class BackgroundTile: public Sprite {
public:
    BackgroundTile(const std::shared_ptr<Texture>& texture,
                   const std::shared_ptr<Shader>& shader,
                   const glm::vec2& size);
    virtual ~BackgroundTile() = default;
    void draw(Renderer& renderer) override;
protected:
    std::shared_ptr<Texture> texture;
    std::shared_ptr<Shader> shader;
    glm::vec2 size;
};



#endif // SPRITE_H
