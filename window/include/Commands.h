#ifndef COMMANDS_H
#define COMMANDS_H
#include "StarshipSprite.h"
#include <optional>
#include <memory>
#include "io.h"


struct Player;
class SpriteFactory;
struct GameplayState;


class GameCommand {
public:
    virtual ~GameCommand() = default;
    virtual void execute(GameplayState& state) =0;
};

class CommandFactory {
public:
    CommandFactory(std::shared_ptr<SpriteFactory> spriteFactory);

    std::shared_ptr<GameCommand> create(const KeyEvent& event) const;
    std::shared_ptr<GameCommand> create(const std::string& message) const;
    ~CommandFactory();
private:
    struct CommandFactoryImpl;
    CommandFactoryImpl* impl;
};



#endif // COMMANDS_H
