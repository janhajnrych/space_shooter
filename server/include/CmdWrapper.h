#ifndef CMDWARPPER_H
#define CMDWARPPER_H

class GameCommand;
class GameState;

class AbstractCommandWrapper {
public:
    virtual void execute(GameState& state, size_t connId) =0;
};


#endif // CMDWARPPER_H
