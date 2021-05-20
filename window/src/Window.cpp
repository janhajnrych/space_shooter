#include "Window.h"
#include <glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SpriteShader.h"
#include "Texture.h"
#include "Renderer.h"
#include "Game.h"
#include <vector>
#include "Sprite.h"
#include <unordered_map>
#include "io.h"
#include "Server.h"

namespace  {

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void initGl(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
}

GLFWwindow* loadGlWindow(const std::string& windowTitle, const Size& size){
    GLFWwindow* window = glfwCreateWindow(static_cast<int>(size.width),
                                          static_cast<int>(size.height),
                                          windowTitle.c_str(), nullptr, nullptr);
    if (window == nullptr)
    {
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    return window;
}

bool loadGlad(){
    return gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
}

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

}

struct Window::WindowImpl {
    std::string title;
    Size size;
    Color bgColor;
    KeyEmitter keyEmitter;
    //MouseEmitter mouseEmitter;
};


Window::Window(const std::string& title,
               const Size& size,
               const Color& bgColor): impl(new WindowImpl{title, size, bgColor, KeyEmitter()}){}

Window::~Window() {
    delete impl;
}

int Window::run(){
    initGl();

    auto window = loadGlWindow(impl->title, impl->size);
    if (window == nullptr){
       std::cerr << "Failed to create GLFW window" << std::endl;
       return -1;
    }
    if (!loadGlad()){
        std::cerr << "Failed to load glad" << std::endl;
        return -2;
    }
    //tcp_server::run();
    Renderer* renderer = new Renderer();
    Game game;
    impl->keyEmitter.subscribe(std::bind(&Game::handleKey, &game, std::placeholders::_1));
    //impl->mouseEmitter.subscribe(std::bind(&Game::handleMouse, &game, std::placeholders::_1));
    if (!game.createRoom())
        return -3;
    auto lastFrame = glfwGetTime();
    decltype(lastFrame) deltaTime = static_cast<decltype(lastFrame)>(0);
    while (!glfwWindowShouldClose(window))
    {

        auto currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        impl->keyEmitter.processInput(window);
        //impl->mouseEmitter.processInput(window);
        glClearColor(impl->bgColor.red, impl->bgColor.green, impl->bgColor.blue, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        game.update(static_cast<float>(deltaTime));
        game.draw(*renderer);
        glfwSwapBuffers(window);
    }
    delete renderer;
    glfwTerminate();
    return 0;
}



