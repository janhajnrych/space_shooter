#include "Messages.h"
#include <vector>
#include <iostream>
#include <unordered_set>

namespace  {

constexpr float SCALE = 1000;
constexpr char SEP = ',';

class StringScanner {
public:
    StringScanner(const std::string& msg, char delimiter): buff(msg), delimiter(delimiter){}

    std::string next(){
        pos = buff.find(delimiter);
        auto token = buff.substr(0, pos);
        buff.erase(0, pos + 1);
        counter++;
        return token;
    }

    bool hasNext() const {
        return pos != std::string::npos;
    }

private:
    std::string buff;
    char delimiter;
    size_t pos = 0;
    int counter = -1;
};

std::shared_ptr<Player> getPlayer(const GameState& state, size_t playerId){
    auto search = state.players.find(playerId);
    if (search == state.players.end())
        return nullptr;
    return search->second;
}

}

std::shared_ptr<SyncPlayerCmd> SyncPlayerCmd::create(const GameState& state, size_t playerId) {
    auto player = getPlayer(state, playerId);
    if (player && !player->ship.expired()) {
        auto sprite = player->ship.lock();
        auto pos = sprite->getPos();
        auto velo = sprite->getVelo();
        auto team = player->team;
        auto rot = sprite->getRotation();
        auto fireRate = sprite->getFireRate();
        return std::shared_ptr<SyncPlayerCmd>(new SyncPlayerCmd(playerId, team, pos, velo, rot, fireRate));
    }
    return nullptr;
}

SyncPlayerCmd::SyncPlayerCmd(size_t playerId, Team team,
                             const glm::vec2& pos, const glm::vec2& velo,
                             float rot, float fireRate):
    pos(pos), velo(velo), playerId(playerId), rot(rot), fireRate(fireRate), team(team) {}


std::string SyncPlayerCmd::serialize() const {
    std::string out = std::to_string(playerId) + SEP + std::to_string(team) + SEP;
    out+= std::to_string(int(pos.x * SCALE)) + SEP + std::to_string(int(pos.y * SCALE)) + SEP;
    out+= std::to_string(int(velo.x * SCALE)) + SEP + std::to_string(int(velo.y * SCALE)) + SEP;
    out+= std::to_string(int(rot * SCALE)) + SEP + std::to_string(int(fireRate));
    return out;
}

void SyncPlayerCmd::execute(GameState& state) {
    auto player = getPlayer(state, playerId);
    if (player && !player->ship.expired()){
        auto sprite = player->ship.lock();
        sprite->setVelo(velo);
        sprite->jumpTo(pos);
        sprite->setRot(rot);
        sprite->setFireRate(fireRate);
    } else {
        auto ship = std::make_shared<StarshipObject>(team, playerId);
        state.players[playerId] = std::make_shared<Player>(playerId, team, ship);
        state.addShip(ship);
    }
}

size_t SyncPlayerCmd::getPlayerId() const {
    return playerId;
}

std::vector<std::string> SyncPlayerCmd::split(const std::string& msg) {
    StringScanner scanner(msg, delimiter);
    std::vector<std::string> tokens;
    while (scanner.hasNext()){
        tokens.push_back(scanner.next());
    }
    return tokens;
}

std::shared_ptr<SyncPlayerCmd> SyncPlayerCmd::parse(const std::string& msg){
    StringScanner scanner(msg, SEP);
    std::vector<int> numbers;
    constexpr size_t maxCapacity = 8;
    numbers.reserve(maxCapacity);
    while (scanner.hasNext()){
        auto token = scanner.next();
        numbers.push_back(atoi(token.c_str()));
        if (numbers.size() == maxCapacity)
           break;
    }
    if (numbers.size() < maxCapacity)
        return nullptr;
    auto playerId = static_cast<size_t>(numbers[0]);
    auto team = static_cast<Team>(numbers[1]);
    auto pos = glm::vec2(numbers[2]/SCALE, numbers[3]/SCALE);
    auto velo = glm::vec2(numbers[4]/SCALE, numbers[5]/SCALE);
    auto rot = float(numbers[6]/SCALE);
    auto fireRate = float(numbers[7]);
    return std::shared_ptr<SyncPlayerCmd>(new SyncPlayerCmd(playerId, team, pos, velo, rot, fireRate));
}

SyncGameCmd::SyncGameCmd() {}

std::shared_ptr<SyncGameCmd> SyncGameCmd::create() {
    return std::shared_ptr<SyncGameCmd>(new SyncGameCmd());
}

std::shared_ptr<SyncGameCmd> SyncGameCmd::create(const GameState &state) {
    auto cmd = create();
    for (auto [playerId, player]: state.players) {
        auto subcmd = SyncPlayerCmd::create(state, playerId);
        if (subcmd)
            cmd->subcommands.push_back(subcmd);
    }
    return cmd;
}

std::shared_ptr<SyncGameCmd> SyncGameCmd::parse(const std::string& msg) {
    auto cmd = create();
    for (const auto& token: SyncPlayerCmd::split(msg)){
        auto subcmd = SyncPlayerCmd::parse(token);
        if (subcmd)
            cmd->subcommands.push_back(subcmd);
    }
    return cmd;
}

std::string SyncGameCmd::serialize() const {
    std::string msg = "";
    for (const auto& subcmd: subcommands){
        msg+= subcmd->serialize() + SyncPlayerCmd::delimiter;
    }
    return msg;
}

void SyncGameCmd::execute(GameState& state) {
    std::unordered_set<size_t> playerIds;
    for (auto& subcmd: subcommands){
        subcmd->execute(state);
        playerIds.insert(subcmd->getPlayerId());
    }
    for (auto it = state.players.begin(); it != state.players.end();) {
       if(playerIds.find(it->first) == playerIds.end()) {
          if (!it->second->ship.expired()) {
            it->second->ship.lock()->die();
          }
          it = state.players.erase(it);
       }
       else
          it++;
    }
}
