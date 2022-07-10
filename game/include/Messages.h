#ifndef MESSAGES_H
#define MESSAGES_H
#include <optional>
#include <memory>
#include <vector>
#include "GameState.h"
#include "GameCommand.h"
#include "Team.h"


struct Player;

class GameMessage: public GameCommand {
public:
    virtual std::string serialize() const = 0;
};

class SyncPlayerCmd: public GameMessage {
public:
    static std::shared_ptr<SyncPlayerCmd> create(const GameState& state, size_t playerId);
    static std::shared_ptr<SyncPlayerCmd> parse(const std::string& msg);
    static std::vector<std::string> split(const std::string& msg);
    std::string serialize() const override;
    void execute(GameState& state) override;
    static constexpr char delimiter = '|';
    size_t getPlayerId() const;
private:
    SyncPlayerCmd(size_t playerId, Team team,
                  const glm::vec2& pos, const glm::vec2& velo,
                  float rot, float fireRate);
    glm::vec2 pos, velo;
    size_t playerId;
    float rot, fireRate;
    Team team;
};

class SyncGameCmd: public GameMessage {
public:
    static std::shared_ptr<SyncGameCmd> create(const GameState& state);
    static std::shared_ptr<SyncGameCmd> parse(const std::string& msg);
    std::string serialize() const override;
    void execute(GameState& state) override;
private:
    SyncGameCmd();
    static std::shared_ptr<SyncGameCmd> create();
    std::vector<std::shared_ptr<SyncPlayerCmd>> subcommands;
};


#endif // MESSAGES_H
