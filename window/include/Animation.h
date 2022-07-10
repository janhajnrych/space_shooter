#ifndef ANIMATION_H
#define ANIMATION_H
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <string>
#include "ObjectSprite.h"
#include "EvolvingObject.h"

class Renderer;
class Texture;
class Shader;

class Animation: public AbstractObjectSprite {
public:
    Animation(std::shared_ptr<Shader> shader,
              std::vector<std::shared_ptr<Texture>> textures,
              std::shared_ptr<EvolvingObject> object);
    virtual ~Animation() override;
    void draw(Renderer& renderer) override;
    std::shared_ptr<DynamicObject> getObject() const override;
protected:
    std::vector<std::shared_ptr<Texture>> textures;
    std::shared_ptr<Shader> shader;
    std::shared_ptr<EvolvingObject> object;
    std::shared_ptr<Texture> getCurrentTexture() const;
};


#endif // ANIMATION_H
