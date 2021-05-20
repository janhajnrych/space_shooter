#ifndef SPRITESHADER_H
#define SPRITESHADER_H
#include "Shader.h"

class SpriteShader: public Shader
{
public:
    SpriteShader();
    void setTransform(const glm::mat4& transformMatrix) override;
    void activate() override;
    void setColor(const glm::vec4& color) override;
};


#endif // SPRITESHADER_H
