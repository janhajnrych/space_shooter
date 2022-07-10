#include "Game.h"
#include <vector>
#include <memory>
#include "Loader.h"
#include "Renderer.h"
#include <vector>
#include <unordered_map>
#include <vector>
#include <list>
#include <iostream>
#include "io.h"
#include "SpriteShader.h"
#include "Sprite.h"
#include <unordered_map>
#include "Animation.h"
#include "Client.h"
#include <deque>
#include "GameCommand.h"
#include "Channel.h"
#include "GameState.h"
#include "ObjectSprite.h"
#include "KeyMapper.h"
#include "Boom.h"
#include "Messages.h"
#include "Laser.h"

namespace  {

    class GameplayState: public GameState {
    public:

        GameplayState(std::shared_ptr<SpriteFactory> spriteFactory): spriteFactory(spriteFactory){}

        void draw(Renderer &renderer) {
            for (auto sprite : sprites)
                sprite->draw(renderer);
        }

        void updateSprites(float dt) {
            auto it = sprites.begin();
            while (it != sprites.end()) {
                auto spritePtr = (*it);
                auto obj = spritePtr->getObject();
                if (obj->isAlive()) {
                    obj->update(dt);
                    checkCollision(obj);
                    it++;
                } else {
                    sprites.erase(it++);
                }
            }
        }

        void addShip(std::shared_ptr<StarshipObject> shipObject) override {
            auto sprite = spriteFactory->createShipSprite(shipObject);
            for (auto& gun: shipObject->getGuns()){
                gun->subscribe([this, shipObject](auto pos, auto velo){
                    //constexpr float range = 22.5f;
                    auto ttl = 10.0f;//range/StarshipObject::maxSpeed;
                    auto laser = std::make_shared<LaserObject>(shipObject->getTeam(), pos, velo, ttl);
                    sprites.push_back(spriteFactory->createLaserSprite(laser));
                });
            }
            sprites.push_back(sprite);
        }

    private:
        std::shared_ptr<SpriteFactory> spriteFactory;
        std::list<std::shared_ptr<AbstractObjectSprite>> sprites;

        void checkCollision(std::shared_ptr<DynamicObject> sprite) {
            for (auto otherDrawable: sprites){
                auto other = otherDrawable->getObject();
                if (sprite == other)
                    continue;
                if (sprite->getType() == ObjectType::Ship && sprite->isPointInside(other->getPos())){
                    if (other->getType() == ObjectType::Laser){
                        other->die();
                        sprite->hit();
                        createBoom(*sprite, *other);
                    }
               }
            }
        }

        void createBoom(const DynamicObject& target, const DynamicObject& proj) {
            auto boomPos = 0.5f * (proj.getPos() + target.getPos());
            auto boomSprite = spriteFactory->createExplosion(boomPos, target.getVelo());
            sprites.push_back(boomSprite);
        }
    };

}

struct Game::GameImpl {
    std::shared_ptr<Sprite> background;
    std::shared_ptr<SpriteShader> shader;
    std::unique_ptr<tcp_client> tcpClient;
    std::shared_ptr<SpriteFactory> spriteFactory;
    std::unique_ptr<KeyMapper> commandFactory;
    std::unique_ptr<GameplayState> state;
    std::deque<std::shared_ptr<GameCommand>> commands;

    void constructWorld(Loader& loader) {
        spriteFactory = loader.createSpriteFactory();
        state = std::make_unique<GameplayState>(spriteFactory);
        background = spriteFactory->createBackground(4.0f);
        shader = loader.getSpriteShader();
        commandFactory = std::make_unique<KeyMapper>();
    }

    void connectToServer() {
        tcpClient = std::make_unique<tcp_client>();
        tcpClient->start();
    }

    void readRemoteMessages() {
        while (tcpClient->has_messages()){
            auto msg = tcpClient->read();
            auto cmd = SyncGameCmd::parse(msg);
            if(cmd)
                commands.push_back(cmd);
        }
    }

    void draw(Renderer &renderer) {
        renderer.start();
        shader->activate();
        background->draw(renderer);
        state->draw(renderer);
    }

    void execCommands() {
        while (!commands.empty()) {
            auto cmd = commands.front();
            cmd->execute(*state);
            commands.pop_front();
        }
    }

};


Game::Game(): impl(new GameImpl()) {}

bool Game::createRoom() {
    Loader loader;
    if (!loader.load())
        return false;
    impl->constructWorld(loader);
    impl->connectToServer();
    return true;
}

void Game::draw(Renderer& renderer) {
    impl->draw(renderer);
}

void Game::update(float dt) {
  impl->readRemoteMessages();
  impl->execCommands();
  impl->state->updateSprites(dt);
}

void Game::handleKey(const KeyEvent& keyEvent) {
    auto msg = impl->commandFactory->mapKey(keyEvent);
    impl->tcpClient->write(msg);
}

Game::~Game() {
    delete impl;
}
