#ifndef COMMANDFACTORY_H
#define COMMANDFACTORY_H
#include <string>
#include <memory>
#include "Team.h"
#include <optional>

class GameCommand;
class GameState;

class ServerCmdFactory {
public:
    ServerCmdFactory(size_t playerId);
    ~ServerCmdFactory();
    std::shared_ptr<GameCommand> create(const std::string& msg) const;
private:
    struct ServerCmdFactoryImpl;
    ServerCmdFactoryImpl* impl;
};


#endif // COMMANDFACTORY_H
