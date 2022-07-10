#ifndef SERVERGAME_H
#define SERVERGAME_H
#include <functional>
#include <string>

class GameCommand;

class Game
{
public:
    Game(std::function<void(const std::string&)> onUpdate);
    ~Game();
    void accept(GameCommand& cmd);
    void run();
    void removePlayer(size_t playerId);
private:
    struct GameImpl;
    GameImpl* impl;
};


#endif // SERVERGAME_H
