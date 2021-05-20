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
#include "Server.h"
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
    void communicateWithClients();
    void checkCollision(std::shared_ptr<DynamicSprite> sprite);
    void startServer();
    void constructWorld(Loader& loader);
    void draw(Renderer& render);
    void createBoom(const DynamicSprite& target, const DynamicSprite& proj);

    std::shared_ptr<CommandFactory> commandFactory;
    std::shared_ptr<Sprite> background;
    std::shared_ptr<SpriteShader> shader;
    std::unique_ptr<tcp_server> server;
    std::shared_ptr<SpriteFactory> spriteFactory;
    std::deque<std::shared_ptr<GameCommand>> commands;
    GameplayState state;
    int commCounter = 0;
    static const int COMMUNICATION_PERIOD = 30;
};

void Game::GameImpl::execCommands() {
    while (!commands.empty()){
        auto cmd = commands.front();
        cmd->execute(state);
        commands.pop_front();
    }
}

void Game::GameImpl::communicateWithClients() {
    commCounter++;
    if (commCounter % COMMUNICATION_PERIOD != 0)
        return;
    server->update();
    for (auto player : state.players) {
        if (player->channel.expired())
            continue;
        auto channel = player->channel.lock();
        while (channel->has_data_to_read()) {
            auto msg = channel->read();
            auto cmd = commandFactory->create(msg);
            if (cmd != nullptr)
                commands.push_back(cmd);
        }
        if (player->sprite.expired())
            continue;
        auto msg = player->sprite.lock()->serialize();
        channel->write("me:" + msg);
        for (auto other : state.players) {
            if (other == player)
                continue;
            auto msg = other->sprite.lock()->serialize();
            channel->write("other:" + msg);
        }
    }
}

void Game::GameImpl::updateSprites(float dt) {
    auto it = state.sprites.begin();
    while (it != state.sprites.end()) {
        auto spritePtr = (*it);
        if (spritePtr->isAlive()) {
            spritePtr->update(dt);
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

void Game::GameImpl::startServer() {
    server = std::make_unique<tcp_server>();
    server->on_new_connection([this](std::shared_ptr<message_channel> channel_ptr){
        if (this->state.players.size() != 0){
            this->state.players.back()->channel = channel_ptr;
        } else {
            auto player = std::make_shared<Player>();
            player->channel = channel_ptr;
            state.players.push_back(player);
        }
    });
    server->start();
}

void Game::GameImpl::constructWorld(Loader& loader) {
    spriteFactory = loader.createSpriteFactory();
    background = spriteFactory->createBackground(4.0f);
    shader = loader.getSpriteShader();
    commandFactory = std::make_shared<CommandFactory>(spriteFactory);
}

Game::Game(): impl(new GameImpl()) {}

bool Game::createRoom() {
    Loader loader;
    if (!loader.load())
        return false;
    impl->constructWorld(loader);
    impl->startServer();
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
  impl->communicateWithClients();
  impl->execCommands();
  impl->updateSprites(dt);
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
