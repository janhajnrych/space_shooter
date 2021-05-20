#include "Commands.h"
#include "Game.h"
#include "Loader.h"
#include "LaserSprite.h"

namespace  {

    class PlayerCommand: public GameCommand {
    public:
        void execute(GameState& state) override {
            auto player = state.getPlayer(issuerId);
            if (player != nullptr)
                execOnPlayer(*player);
        }

        virtual void execOnPlayer(Player& player) =0;

        std::shared_ptr<PlayerCommand> withTarget(unsigned id) {
            auto cloned = clone();
            cloned->issuerId = id;
            return cloned;
        }

        virtual std::shared_ptr<PlayerCommand> clone() const =0;

    private:
        unsigned issuerId = 0;
    };

    class ShipCommand: public PlayerCommand {
    public:
        void execOnPlayer(Player& player) override {
            auto sprite = player.sprite.lock();
            execOnShip(*sprite);
        }

        virtual void execOnShip(StarshipSprite& sprite) =0;
    };

    class SteerCommand: public ShipCommand {
    public:
        SteerCommand(float torque): torque(torque){}

        void execOnShip(StarshipSprite& sprite) override {
            sprite.setSteeringTorque(torque);
        }

        std::shared_ptr<PlayerCommand> clone() const override {
            return std::make_shared<SteerCommand>(*this);
        }

    private:
        float torque;
    };


    class EngineCommand: public ShipCommand {
    public:
        EngineCommand(float thrust): thrust(thrust){}

        void execOnShip(StarshipSprite& sprite) override {
            sprite.setLinearThrust(thrust);
        }

        std::shared_ptr<PlayerCommand> clone() const override {
            return std::make_shared<EngineCommand>(*this);
        }
    private:
        float thrust;
    };

    class GunCommand: public ShipCommand {
    public:
        GunCommand(float fireRate): fireRate(fireRate){}

        void execOnShip(StarshipSprite& sprite) override {
            sprite.setFireRate(fireRate);
        }

        std::shared_ptr<PlayerCommand> clone() const override {
            return std::make_shared<GunCommand>(*this);
        }
    private:
        float fireRate;
    };

    class CreateShipCmd: public GameCommand {
    public:
        CreateShipCmd(std::unique_ptr<StarshipSprite> shipProto,
                      std::shared_ptr<LaserSprite> laserProto):
            shipPrototype(std::move(shipProto)), laserPrototype(laserProto){}

        void execute(GameState& state) override {
            state.addSprite(createFromPrototype(state));
        }

        std::shared_ptr<StarshipSprite> createFromPrototype(GameState& state){
            auto ship = std::make_shared<StarshipSprite>(*shipPrototype);

            ship->getWeapon().subscribe([&state, this](const FireEvent& event){
                auto laser = std::make_shared<LaserSprite>(*laserPrototype);
                laser->activate(event.pos, event.velo, event.ttl);
                state.addSprite(laser);
            });
            return ship;
        }


    private:
        std::unique_ptr<StarshipSprite> shipPrototype;
        std::shared_ptr<LaserSprite> laserPrototype;
    };

    class CreatePlayerCmd: public GameCommand {
    public:

        CreatePlayerCmd(std::shared_ptr<CreateShipCmd> next): next(next){}

        void execute(GameState& state) override {
            auto player = std::make_shared<Player>();
            state.addPlayer(player);
            auto ship = next->createFromPrototype(state);
            state.addSprite(ship);
            player->sprite = std::weak_ptr<StarshipSprite>(ship);
        }

    private:
        std::shared_ptr<CreateShipCmd> next;
    };

    class CreateFirstPlayerCmd: public CreatePlayerCmd {
    public:

        CreateFirstPlayerCmd(std::shared_ptr<CreateShipCmd> next): CreatePlayerCmd(next) {}

        void execute(GameState& state) override {
            state.reset();
            CreatePlayerCmd::execute(state);
        }

    };

    using CreateShipCmdPtr = std::shared_ptr<CreateShipCmd>;

    std::tuple<CreateShipCmdPtr, CreateShipCmdPtr> buildShipCommands(SpriteFactory& spriteFactory, SpriteFactory::Sprite_t type){
        auto ship = spriteFactory.createStarship(type, glm::vec2(-0.45f, -0.45f), 0.0f);
        std::shared_ptr<LaserSprite> laser = spriteFactory.createLaser(type);
        auto createAsFriend = std::make_shared<CreateShipCmd>(std::move(ship), laser);
        auto otherShip = spriteFactory.createStarship(type, glm::vec2(0.45f, 0.45f), 0.0f);
        auto createAsEnemy = std::make_shared<CreateShipCmd>(std::move(otherShip), laser);
        return std::make_tuple(createAsFriend, createAsEnemy);
    }

}

CommandFactory::CommandFactory(std::shared_ptr<SpriteFactory> spriteFactory){
    auto releaseAcceleration = std::make_shared<EngineCommand>(0);
    auto releaseSteering = std::make_shared<SteerCommand>(0);
    auto speedUp = std::make_shared<EngineCommand>(StarshipSprite::maxThrust);
    auto slowDown = std::make_shared<EngineCommand>(-StarshipSprite::maxThrust);
    auto turnLeft = std::make_shared<SteerCommand>(StarshipSprite::maxTorque);
    auto turnRight = std::make_shared<SteerCommand>(-StarshipSprite::maxTorque);
    auto openFire = std::make_shared<GunCommand>(StarshipSprite::maxFireRate);
    auto holdFire = std::make_shared<GunCommand>(0);

    keyToCmd[KeyEvent{ActionKey_t::Up, State_t::Released}] = releaseAcceleration;
    keyToCmd[KeyEvent{ActionKey_t::Down, State_t::Released}] = releaseAcceleration;
    keyToCmd[KeyEvent{ActionKey_t::Left, State_t::Released}] = releaseSteering;
    keyToCmd[KeyEvent{ActionKey_t::Right, State_t::Released}] = releaseSteering;
    keyToCmd[KeyEvent{ActionKey_t::Fire, State_t::Released}] = holdFire;
    keyToCmd[KeyEvent{ActionKey_t::Up, State_t::Pressed}] = speedUp;
    keyToCmd[KeyEvent{ActionKey_t::Down, State_t::Pressed}] = slowDown;
    keyToCmd[KeyEvent{ActionKey_t::Left, State_t::Pressed}] = turnLeft;
    keyToCmd[KeyEvent{ActionKey_t::Right, State_t::Pressed}] = turnRight;
    keyToCmd[KeyEvent{ActionKey_t::Fire, State_t::Pressed}] = openFire;

    auto [crTieCmd, crTieCmdOther] = buildShipCommands(*spriteFactory, SpriteFactory::Sprite_t::tie_s);
    keyToCmd[KeyEvent{ActionKey_t::StartY, State_t::Pressed}] = std::make_shared<CreateFirstPlayerCmd>(crTieCmd);
    keyToCmd[KeyEvent{ActionKey_t::StartX, State_t::Released}] = std::make_shared<CreatePlayerCmd>(crTieCmdOther);

    auto [crXCmd, crXCmdOther] = buildShipCommands(*spriteFactory, SpriteFactory::Sprite_t::xWing_s);
    keyToCmd[KeyEvent{ActionKey_t::StartX, State_t::Pressed}] = std::make_shared<CreateFirstPlayerCmd>(crXCmd);
    keyToCmd[KeyEvent{ActionKey_t::StartY, State_t::Released}] = std::make_shared<CreatePlayerCmd>(crXCmdOther);

    msgToCmd["w0"] = releaseAcceleration->withTarget(1);
    msgToCmd["s0"] = releaseAcceleration->withTarget(1);
    msgToCmd["a0"] = releaseSteering->withTarget(1);
    msgToCmd["d0"] = releaseSteering->withTarget(1);
    msgToCmd["k0"] = holdFire->withTarget(1);
    msgToCmd["w1"] = speedUp->withTarget(1);
    msgToCmd["s1"] = slowDown->withTarget(1);
    msgToCmd["a1"] = turnLeft->withTarget(1);
    msgToCmd["d1"] = turnRight->withTarget(1);
    msgToCmd["k1"] = openFire->withTarget(1);
    //msgToCmd["t1"] = keyToCmd[KeyEvent{ActionKey_t::StartY, State_t::Pressed}];
    //msgToCmd["t0"] = keyToCmd[KeyEvent{ActionKey_t::StartY, State_t::Released}];
    //msgToCmd["y1"] = keyToCmd[KeyEvent{ActionKey_t::StartX, State_t::Pressed}];
    //msgToCmd["y0"] = keyToCmd[KeyEvent{ActionKey_t::StartX, State_t::Released}];
}

std::shared_ptr<GameCommand> CommandFactory::create(const KeyEvent& event) const {
    if (keyToCmd.count(event) == 0)
        return nullptr;
    return keyToCmd.at(event);
}

std::shared_ptr<GameCommand> CommandFactory::create(const std::string& message) const {
    if (msgToCmd.count(message) == 0)
        return nullptr;
    return msgToCmd.at(message);
}
