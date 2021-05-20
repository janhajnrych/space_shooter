#ifndef KEYEMITTER_H
#define KEYEMITTER_H
#include "Events.h"

enum ActionKey_t {
    Up, Down, Left, Right, Fire, Close, StartX, StartY
};

enum State_t {
    Pressed, Released
};

struct KeyEvent {
    ActionKey_t key;
    State_t state;
    bool operator==(const KeyEvent& otherEvent) const{
       return (this->key == otherEvent.key && this->state == otherEvent.state);
    } // needed for mapping
};

class GLFWwindow;

class KeyEmitter: public EventEmitter<KeyEvent>
{
public:
    KeyEmitter();
    virtual ~KeyEmitter();
    void processInput(GLFWwindow* window);
private:
    struct KeyEmitterImpl;
    KeyEmitterImpl* impl;
};

enum MouseButton_t {
    LeftMouseButton, RightMouseButton
};

struct MouseEvent {
    double x, y;
    MouseButton_t button;
    State_t pressed;
};

class MouseEmitter: public EventEmitter<MouseEvent>
{
public:
    MouseEmitter();
    virtual ~MouseEmitter();
    void processInput(GLFWwindow* window);
private:
    struct MouseEmitterImpl;
    MouseEmitterImpl* impl;
};

#endif // KEYEMITTER_H
