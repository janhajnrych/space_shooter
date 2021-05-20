#include "Shader.h"
#include <iostream>

struct Shader::ShaderImpl {
    void createProgram() {
        const auto vertexShader = createShader(vertexShaderSource.c_str(), GL_VERTEX_SHADER);
        const auto fragmentShader = createShader(fragmentShaderSource.c_str(), GL_FRAGMENT_SHADER);
        if (!vertexShader || !fragmentShader){
            state = CompilationFailed;
            return;
        }
        GLuint shaderProgramId = glCreateProgram();
        glAttachShader(shaderProgramId, vertexShader);
        glAttachShader(shaderProgramId, fragmentShader);
        linkProgram(shaderProgramId);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        state = Linked;
        id = shaderProgramId;
    }
    enum State_t {
        NotLoaded,
        Compiled,
        CompilationFailed,
        LinkingFailed,
        Linked
    };

    State_t state = NotLoaded;
    GLuint id = 0;
    std::string vertexShaderSource;
    std::string fragmentShaderSource;

private:

    void linkProgram(GLuint shaderProgramId) {
        glLinkProgram(shaderProgramId);
        // check for linking errors
        int success;
        char infoLog[512];
        glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgramId, 512, nullptr, infoLog);
            std::cout << "shader linking failed" << infoLog << std::endl;
            state = LinkingFailed;
            return;
        }
    }

    void compileShader(GLuint shaderId, GLuint type) {
        glCompileShader(shaderId);
        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shaderId, 512, nullptr, infoLog);
            const char* shaderTypeName = type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT";
            std::cout << shaderTypeName << " shader compilation failed " << infoLog << std::endl;
            state = CompilationFailed;
        } else {
            state = Compiled;
        }
    }

    GLuint createShader(const char* source, GLuint type) {
        GLuint shaderId = glCreateShader(type);
        glShaderSource(shaderId, 1, &source, nullptr);
        compileShader(shaderId, type);
        return shaderId;
    }

};

Shader::Shader(): impl(new ShaderImpl()) {}

Shader::~Shader() {
    delete impl;
}

GLuint Shader::getId() const {
    return impl->id;
}

bool Shader::isValid() const {
    return impl->state == ShaderImpl::State_t::Linked;
}

void Shader::compile() {
    impl->createProgram();
}

void Shader::setUniform(const std::string& name, GLint value) {
    glUniform1i(glGetUniformLocation(impl->id, name.c_str()), value);
}

void Shader::setUniform(const std::string& name, const glm::mat4& matrix) {
    const auto transformLoc = glGetUniformLocation(impl->id, name.c_str());
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setUniform(const std::string& name, const glm::vec4& vect) {
    const auto transformLoc = glGetUniformLocation(impl->id, name.c_str());
    glUniform4f(transformLoc, vect.x, vect.y, vect.z, vect.w);
}

void Shader::setVertexShaderCode(const std::string& code) {
    impl->vertexShaderSource = code;
}

void Shader::setFragmentShaderCode(const std::string& code) {
    impl->fragmentShaderSource = code;
}

void Shader::activate() {
    glUseProgram(getId());
}
