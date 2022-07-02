#include "io.h"
#include <glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <unordered_set>


struct KeyEmitter::KeyEmitterImpl {
public:

    std::unordered_map<int, ActionKey_t> keyMap = {
        {GLFW_KEY_W, ActionKey_t::Up},
        {GLFW_KEY_S, ActionKey_t::Down},
        {GLFW_KEY_A, ActionKey_t::Left},
        {GLFW_KEY_D, ActionKey_t::Right},

        {GLFW_KEY_K, ActionKey_t::Fire},
        {GLFW_KEY_ESCAPE, ActionKey_t::Close},
        {GLFW_KEY_Y, ActionKey_t::StartX},
        {GLFW_KEY_T, ActionKey_t::StartT}

    };

    std::unordered_set<ActionKey_t> keysPressed;
    std::unordered_set<ActionKey_t> keysReleased;

};

KeyEmitter::KeyEmitter(): impl(new KeyEmitterImpl()) {}

KeyEmitter::~KeyEmitter() {
    delete impl;
}

void KeyEmitter::processInput(GLFWwindow* window) {
    for (const auto& pair: impl->keyMap) {
        const auto isPressed = glfwGetKey(window, pair.first) == GLFW_PRESS;
        const auto key = pair.second;
        if (isPressed){
            impl->keysPressed.insert(key);
            impl->keysReleased.erase(key);
            emit({key, State_t::Pressed});
        }
        else if (impl->keysPressed.find(key) != impl->keysPressed.end()){
            impl->keysReleased.insert(key);
            impl->keysPressed.erase(key);
            emit({key, State_t::Released});
        }
    }
}

struct MouseEmitter::MouseEmitterImpl {
public:

    std::unordered_map<int, MouseButton_t> keyMap = {
        {GLFW_MOUSE_BUTTON_LEFT, MouseButton_t::LeftMouseButton},
        {GLFW_MOUSE_BUTTON_RIGHT, MouseButton_t::RightMouseButton},
    };

    std::unordered_set<MouseButton_t> keysPressed;
    std::unordered_set<MouseButton_t> keysReleased;

};

MouseEmitter::MouseEmitter(): impl(new MouseEmitterImpl()) {}

MouseEmitter::~MouseEmitter() {
    delete impl;
}

void MouseEmitter::processInput(GLFWwindow* window) {
    for (const auto& pair: impl->keyMap) {
        const auto isPressed = glfwGetMouseButton(window, pair.first) == GLFW_PRESS;
        const auto buttonId = pair.second;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        if (isPressed){
            impl->keysPressed.insert(buttonId);
            impl->keysReleased.erase(buttonId);
            emit({xpos, ypos, MouseButton_t::LeftMouseButton, State_t::Pressed});
        }
        else if (impl->keysPressed.find(buttonId) != impl->keysPressed.end()){
            impl->keysReleased.insert(buttonId);
            impl->keysPressed.erase(buttonId);
            emit({xpos, ypos, MouseButton_t::RightMouseButton, State_t::Released});
        }
    }
}



