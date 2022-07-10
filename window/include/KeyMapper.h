#ifndef COMMANDFACTORY_H
#define COMMANDFACTORY_H
#include <string>
#include <memory>
#include "Team.h"
#include <optional>

struct KeyEvent;
class GameCommand;

class KeyMapper {
public:
    KeyMapper();
    std::string mapKey(const KeyEvent& event) const;
private:
    struct KeyMapperImpl;
    KeyMapperImpl* impl;
};


#endif // COMMANDFACTORY_H
