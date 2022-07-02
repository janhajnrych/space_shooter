#ifndef GAME_H
#define GAME_H

struct KeyEvent;
class Renderer;

class Game
{
public:
    Game();
    ~Game();
    bool createRoom();
    void draw(Renderer& renderer);
    void update(float dt);
    void handleKey(const KeyEvent& keyEvent);
private:
    struct GameImpl;
    GameImpl* impl;
};


#endif // GAME_H
