#include "Renderer.h"
#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct Renderer::RendererImpl {

    RendererImpl(){}

    void createVertices(GLuint* VAO, float n=1, float dx=0.5f, float dy=0.5f) {
        float vertices[] = {
             // positions            // texture coords
             dx,  dy,       n, n, // top right
             dx, -dy,       n, 0.0f, // bottom right
            -dx, -dy,       0.0f, 0.0f, // bottom left
            -dx,  dy,       0.0f, n  // top left
        };
        unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        glGenVertexArrays(1, VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(*VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    ~RendererImpl() {
        glDeleteVertexArrays(1, &VAO_sprites);
        glDeleteBuffers(1, &VBO);
    }

private:
    GLuint VBO=0, EBO=0;
public:
    GLuint VAO_sprites=0, VAO_background=1;
};

Renderer::Renderer(): impl(new RendererImpl()){
    impl->createVertices(&impl->VAO_sprites, 1, 1, 1);
    impl->createVertices(&impl->VAO_background, N_BG_REPEAT, 1, 1);
}

Renderer::~Renderer() {
    delete impl;
}

void Renderer::renderSprites() {
    glBindVertexArray(impl->VAO_sprites);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Renderer::renderBackground() {
    glBindVertexArray(impl->VAO_background);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Renderer::start() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
