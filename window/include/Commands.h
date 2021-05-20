#ifndef COMMANDS_H
#define COMMANDS_H
#include "StarshipSprite.h"
#include <memory>
#include <unordered_map>
#include "io.h"
#include <list>

class Player;
class SpriteFactory;
class GameState;

class GameCommand {
public:
    virtual ~GameCommand() = default;
    virtual void execute(GameState& state) =0;
};

class CommandFactory {
public:
    CommandFactory(std::shared_ptr<SpriteFactory> spriteFactory);

    std::shared_ptr<GameCommand> create(const KeyEvent& event) const;
    std::shared_ptr<GameCommand> create(const std::string& message) const;
private:
    struct KeyEventHashFunction {
      size_t operator()(const KeyEvent& event) const {
        size_t rowHash = std::hash<int>()(event.key);
        size_t colHash = std::hash<int>()(event.state) << 1;
        return rowHash ^ colHash;
      }
    };
    std::unordered_map<KeyEvent, std::shared_ptr<GameCommand>, KeyEventHashFunction> keyToCmd;
    std::unordered_map<std::string, std::shared_ptr<GameCommand>> msgToCmd;
};



#endif // COMMANDS_H
