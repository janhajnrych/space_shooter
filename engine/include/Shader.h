#ifndef SHADER_H
#define SHADER_H
#include <glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    Shader();
    virtual ~Shader();
    GLuint getId() const;
    bool isValid() const;
    virtual void activate();
    virtual void setTransform(const glm::mat4& transformMatrix) =0;
    virtual void setColor(const glm::vec4& color) =0;
protected:
    void compile();
    void setUniform(const std::string& name, GLint value);
    void setUniform(const std::string& name, const glm::mat4& matrix);
    void setUniform(const std::string& name, const glm::vec4& vect);
    void setVertexShaderCode(const std::string& code);
    void setFragmentShaderCode(const std::string& code);
private:
    struct ShaderImpl;
    ShaderImpl* impl;
};


#endif // SHADER_H
