#include <vector>
#include <memory>
#include <list>
#include <iostream>
#include <unordered_map>
#include <string>
#include "CommandFactory.h"
#include "GameCommand.h"
#include "CmdWrapper.h"
#include "Starship.h"
#include "GameState.h"

namespace  {

    class CreatePlayerCmd: public GameCommand {
    public:
        CreatePlayerCmd(size_t playerId, Team team): playerId(playerId), team(team){}

        void execute(GameState& state) override {
            auto ship = std::make_shared<StarshipObject>(team, playerId);
            state.players[playerId] = std::make_shared<Player>(playerId, team, ship);
            state.addShip(ship);
        }

    private:
        size_t playerId;
        Team team;
    };

    class ShipCommand: public GameCommand {
    public:
        ShipCommand(size_t playerId): playerId(playerId){}

        void execute(GameState& state) override {
            auto search = state.players.find(playerId);
            if (search == state.players.end())
                return;
            auto player = search->second;
            if (player->ship.expired())
                return;
            auto sprite = player->ship.lock();
            execOnShip(*sprite);
        }
    protected:
        virtual void execOnShip(StarshipObject& ship) =0;
    private:
        size_t playerId;
    };

    class SteerCommand: public ShipCommand {
    public:
        SteerCommand(size_t playerId, float torque): ShipCommand(playerId), torque(torque){}
    protected:
        void execOnShip(StarshipObject& ship) override {
            ship.setSteeringTorque(torque);
        }
    private:
        float torque;
    };


    class EngineCommand: public ShipCommand {
    public:
        EngineCommand(size_t playerId, float thrust): ShipCommand(playerId), thrust(thrust){}
    protected:
        void execOnShip(StarshipObject& ship) override {
            ship.setLinearThrust(thrust);
        }
    private:
        float thrust;
    };

    class GunCommand: public ShipCommand {
    public:
        GunCommand(size_t playerId, float fireRate): ShipCommand(playerId), fireRate(fireRate){}
    protected:
        void execOnShip(StarshipObject& ship) override {
            ship.setFireRate(fireRate);
        }
    private:
        float fireRate;
    };

//    class RestartCmd: public GameCommand {
//    public:
//        RestartCmd();
//        void execute(GameState& state) override;
//    };


}

struct ServerCmdFactory::ServerCmdFactoryImpl {
    template<typename T>
    using CmdMap = std::unordered_map<T, std::shared_ptr<GameCommand>>;

    CmdMap<std::string> msgToCmd;

    static CmdMap<std::string> createPlayerCmdMap(size_t playerId) {
        CmdMap<std::string> map;
        auto releaseAcceleration = std::make_shared<EngineCommand>(playerId, 0);
        auto releaseSteering = std::make_shared<SteerCommand>(playerId, 0);
        auto speedUp = std::make_shared<EngineCommand>(playerId, StarshipObject::maxThrust);
        auto slowDown = std::make_shared<EngineCommand>(playerId, -StarshipObject::maxThrust);
        auto turnLeft = std::make_shared<SteerCommand>(playerId, StarshipObject::maxTorque);
        auto turnRight = std::make_shared<SteerCommand>(playerId, -StarshipObject::maxTorque);
        auto openFire = std::make_shared<GunCommand>(playerId, StarshipObject::maxFireRate);
        auto holdFire = std::make_shared<GunCommand>(playerId, StarshipObject::supressedFireRate);
        auto createT = std::make_shared<CreatePlayerCmd>(playerId, Team::Red);
        auto createX = std::make_shared<CreatePlayerCmd>(playerId, Team::Blue);
        map["w0"] = releaseAcceleration;
        map["s0"] = releaseAcceleration;
        map["a0"] = releaseSteering;
        map["d0"] = releaseSteering;
        map["k0"] = holdFire;
        map["w1"] = speedUp;
        map["s1"] = slowDown;
        map["a1"] = turnLeft;
        map["d1"] = turnRight;
        map["k1"] = openFire;
        map["t1"] = createT;
        map["x1"] = createX;
        return map;
    }

    ServerCmdFactoryImpl(size_t playerId){
        msgToCmd = createPlayerCmdMap(playerId);
    }
};

ServerCmdFactory::ServerCmdFactory(size_t playerId): impl(new ServerCmdFactoryImpl(playerId)) {}

ServerCmdFactory::~ServerCmdFactory(){
    delete impl;
}

std::shared_ptr<GameCommand> ServerCmdFactory::create(const std::string& msg) const {
    if (impl->msgToCmd.count(msg) == 0)
        return nullptr;
    return impl->msgToCmd.at(msg);
}



