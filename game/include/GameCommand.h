#ifndef COMMANDS_H
#define COMMANDS_H
#include <memory>

struct GameState;
struct Player;

class GameCommand {
public:
    virtual ~GameCommand() = default;
    virtual void execute(GameState& state) =0;
};







#endif // COMMANDS_H
