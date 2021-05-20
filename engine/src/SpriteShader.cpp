#include "SpriteShader.h"

SpriteShader::SpriteShader() {
    std::string vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec2 aPos;\n"
                                     "layout (location = 1) in vec2 aTexCoord;\n"
                                     "out vec2 TexCoord;\n"
                                     "uniform mat4 transform;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = transform * vec4(aPos, 0.0, 1.0);\n"
                                     "   TexCoord = aTexCoord;\n"
                                     "}\0";
    Shader::setVertexShaderCode(vertexShaderSource);
    std::string fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "in vec2 TexCoord;\n"
                                       "uniform sampler2D spriteTexture;\n"
                                       "uniform vec4 spriteColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = texture(spriteTexture, TexCoord) * spriteColor;\n"
                                       "}\n\0";
    Shader::setFragmentShaderCode(fragmentShaderSource);
    Shader::compile();
}

void SpriteShader::activate() {
    Shader::activate();
    Shader::setUniform("spriteTexture", 0);
    Shader::setUniform("spriteColor", glm::vec4(0, 0, 0, 1));
}

void SpriteShader::setTransform(const glm::mat4& transformMatrix) {
    Shader::setUniform("transform", transformMatrix);
}

void SpriteShader::setColor(const glm::vec4& color) {
    Shader::setUniform("spriteColor", color);
}
