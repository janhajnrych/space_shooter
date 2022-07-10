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
protected:
    static glm::mat4 buildTransform(const glm::vec2& pos, float rotation, const glm::vec2& size);
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
