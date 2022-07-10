#ifndef OBJECTSPRITE_H
#define OBJECTSPRITE_H
#include "Sprite.h"
#include "DynamicObject.h"

class AbstractObjectSprite: public Sprite {
public:
    virtual std::shared_ptr<DynamicObject> getObject() const =0;
};

class ColoredObjectSprite: public AbstractObjectSprite
{
public:
    ColoredObjectSprite(std::shared_ptr<Shader> shader,
                        std::shared_ptr<Texture> texture,
                        std::shared_ptr<DynamicObject> object,
                        const glm::vec4& color);
    void draw(Renderer& renderer) override;
    std::shared_ptr<DynamicObject> getObject() const override;
private:
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture> texture;
    std::shared_ptr<DynamicObject> object;
    glm::vec4 color;
};


#endif // OBJECTSPRITE_H
