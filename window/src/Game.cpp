#include "Game.h"
#include <vector>
#include "StarshipSprite.h"
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
#include "LaserSprite.h"
#include "Animation.h"
#include "Client.h"
#include <deque>
#include "Commands.h"
#include "Channel.h"

class GameplayState: public GameState {
public:

    virtual ~GameplayState() =default;

    void addSprite(std::shared_ptr<DynamicSprite> sprite) override {
        sprites.push_back(sprite);
    }

    void addPlayer(std::shared_ptr<Player> player) override {
        player->id = static_cast<unsigned>(players.size());
        players.push_back(player);
    }

    void reset() override {
        sprites.clear();
        players.clear();
    }

    std::shared_ptr<Player> getPlayer(unsigned id) const override {
        if (players.size() <= id)
            return nullptr;
        return players.at(id);
    }

    std::list<std::shared_ptr<DynamicSprite>> sprites;
    std::vector<std::shared_ptr<Player>> players;


};

struct Game::GameImpl {
    void updateSprites(float dt);
    void execCommands();
    void checkCollision(std::shared_ptr<DynamicSprite> sprite);
    void connectToServer();
    void constructWorld(Loader& loader);
    void draw(Renderer& render);
    void createBoom(const DynamicSprite& target, const DynamicSprite& proj);
    std::string gameStateBuffer;
    void updateBuffer(const DynamicSprite& sprite);
    void readRemoteMessages();
    void sendState(float dt);

    std::shared_ptr<CommandFactory> commandFactory;
    std::shared_ptr<Sprite> background;
    std::shared_ptr<SpriteShader> shader;
    std::unique_ptr<tcp_client> tcpClient;
    std::shared_ptr<SpriteFactory> spriteFactory;
    std::deque<std::shared_ptr<GameCommand>> commands;
    GameplayState state;
    float commCounter = 0.0;
    static constexpr float COMMUNICATION_PERIOD = 1.0;
};

void Game::GameImpl::execCommands() {
    while (!commands.empty()){
        auto cmd = commands.front();
        cmd->execute(state);
        commands.pop_front();
    }
}

void Game::GameImpl::readRemoteMessages() {
    while (tcpClient->has_messages()){
        auto msg = tcpClient->read();
        auto cmd = commandFactory->create(msg);
        if (cmd)
          commands.push_back(cmd);
    }
}

void Game::GameImpl::sendState(float dt) {
    if (commCounter > COMMUNICATION_PERIOD){
        commCounter = 0.0;
        if (gameStateBuffer.size() != 0) {
            tcpClient->write(gameStateBuffer);
        }
    } else {
      commCounter+= dt;
    }
}


void Game::GameImpl::updateBuffer(const DynamicSprite& sprite) {
    auto pos = sprite.getPos();
    int x = static_cast<int>(pos.x * 1000);
    int y = static_cast<int>(pos.y * 1000);
    gameStateBuffer+= std::to_string(x) + "," + std::to_string(y) + ",";
    auto velo = sprite.getVelo();
    x = static_cast<int>(velo.x * 1000);
    y = static_cast<int>(velo.y * 1000);
    gameStateBuffer+= std::to_string(x) + "," + std::to_string(y) + ";";
}

void Game::GameImpl::updateSprites(float dt) {
    gameStateBuffer = std::string();
    auto it = state.sprites.begin();
    while (it != state.sprites.end()) {
        auto spritePtr = (*it);
        if (spritePtr->isAlive()) {
            spritePtr->update(dt);
            if (spritePtr->isTrackable()){
                updateBuffer(*spritePtr);
            }
            checkCollision(spritePtr);
            it++;
        } else {
            state.sprites.erase(it++);
        }
    }
}

void Game::GameImpl::createBoom(const DynamicSprite& target, const DynamicSprite& proj) {
    auto boomPos = 0.5f * (proj.getPos() + target.getPos());
    state.sprites.push_back(spriteFactory->createExplosion(boomPos, target.getVelo()));
}

void Game::GameImpl::checkCollision(std::shared_ptr<DynamicSprite> sprite) {
    for (auto other: state.sprites){
        if (sprite == other)
            continue;
        auto proj = dynamic_cast<LaserSprite*>(other.get());
        if (proj == nullptr)
            continue;
        if (sprite->isPointInside(proj->getPos())){
            proj->die();
            createBoom(*sprite, *proj);
            auto ship = dynamic_cast<StarshipSprite*>(sprite.get());
            if (ship == nullptr)
                continue;
            ship->hit();
       }
    }
}

void Game::GameImpl::connectToServer() {
    tcpClient = std::make_unique<tcp_client>();
    tcpClient->start();
}

void Game::GameImpl::constructWorld(Loader& loader) {
    spriteFactory = loader.createSpriteFactory();
    background = spriteFactory->createBackground(4.0f);
    shader = loader.getSpriteShader();
    commandFactory = std::make_shared<CommandFactory>(spriteFactory);
}

Game::Game(): impl(new GameImpl()) {
    impl->gameStateBuffer.reserve(128);
}

bool Game::createRoom() {
    Loader loader;
    if (!loader.load())
        return false;
    impl->constructWorld(loader);
    impl->connectToServer();
    return true;
}

void Game::GameImpl::draw(Renderer &renderer) {
    renderer.start();
    shader->activate();
    background->draw(renderer);
    for (const auto& spirite: state.sprites) {
        spirite->draw(renderer);
    }
}

void Game::draw(Renderer& renderer) {
    impl->draw(renderer);
}

void Game::update(float dt) {
  impl->readRemoteMessages();
  impl->execCommands();
  impl->updateSprites(dt);
  impl->sendState(dt);
}

void Game::handleKey(const KeyEvent& keyEvent) {
    auto cmd = impl->commandFactory->create(keyEvent);
    if (cmd == nullptr)
        return;
    impl->commands.push_back(cmd);
}

Game::~Game() {
    delete impl;
}
