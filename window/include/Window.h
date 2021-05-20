#ifndef WINDOW_H
#define WINDOW_H
#include <string>

struct Size {
    unsigned width, height;
};

class Window
{
public:
    struct Color {
        float red, green, blue;
    };

    Window(const std::string& title, const Size& size, const Color& bgColor);
    ~Window();
    int run();
private:
    struct WindowImpl;
    WindowImpl* impl;
};


#endif // WINDOW_H
