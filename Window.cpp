#include "Window.h"
#include <glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace  {

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

void initGl(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
}

GLFWwindow* loadGlWindow(const std::string& windowTitle, const Window::Size& size){
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
    return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

void loop(GLFWwindow* window, const Window::Color& bgColor) {
    processInput(window);
    glClearColor(bgColor.red, bgColor.green, bgColor.blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();
}

}

Window::Window(const std::string& title,
               const Size& size,
               const Color& bgColor): title(title), size(size), bgColor(bgColor){}

int Window::run(){
    initGl();
    auto window = loadGlWindow(title, size);
    if (window == nullptr){
       std::cerr << "Failed to create GLFW window" << std::endl;
       return -1;
    }
    if (!loadGlad())
        return -1;
    while (!glfwWindowShouldClose(window))
    {
        loop(window, bgColor);
    }
    glfwTerminate();
    return 0;
}



