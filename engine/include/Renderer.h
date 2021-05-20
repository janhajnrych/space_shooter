#ifndef RENDERER_H
#define RENDERER_H

class Renderer
{
public:
    static const int N_BG_REPEAT = 8;
    Renderer();
    ~Renderer();
    void renderSprites();
    void renderBackground();
    void start();
private:
    struct RendererImpl;
    RendererImpl* impl;
};


#endif // RENDERER_H
