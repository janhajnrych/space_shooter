#ifndef GAME_H
#define GAME_H
#include <string>
#include <memory>

struct KeyEvent;
struct MouseEvent;
class Renderer;
class Loader;
class DynamicSprite;

class message_channel;
class StarshipSprite;

struct Player {
    std::weak_ptr<StarshipSprite> sprite;
    unsigned id;
    unsigned score = 0;
    //~Player();
};

class GameState {
public:
    virtual void addSprite(std::shared_ptr<DynamicSprite> sprite) =0;
    virtual void addPlayer(std::shared_ptr<Player> player) =0;
    virtual std::shared_ptr<Player> getPlayer(unsigned id) const =0;
    virtual void reset() =0;
};

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
