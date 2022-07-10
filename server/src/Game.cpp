#include "Game.h"
#include <vector>
#include <memory>
#include <vector>
#include <unordered_map>
#include <vector>
#include <list>
#include <iostream>
#include <unordered_map>
#include <deque>
#include "Channel.h"
#include "GameState.h"
#include "GameCommand.h"
#include <thread>
#include <chrono>
#include <atomic>
#include "Messages.h"

namespace  {

    struct GameplayState: public GameState {
    public:

        GameplayState(std::function<void(const std::string&)> onUpdate): onUpdate(onUpdate){}

        void updateSprites(float dt) {
            auto it = sprites.begin();
            while (it != sprites.end()) {
                auto spritePtr = (*it);
                auto obj = spritePtr;
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
            sprites.push_back(shipObject);
        }

        void makeMsg(){
            std::string msg = "";
            auto cmd = SyncGameCmd::create(*this);
            if (cmd)
                msg+= cmd->serialize();
            if (msg.size() != 0)
                onUpdate(msg);
        }

    private:
        std::list<std::shared_ptr<DynamicObject>> sprites;
        std::optional<Team> hostTeam;
        std::function<void(const std::string&)> onUpdate;

        void checkCollision(std::shared_ptr<DynamicObject> sprite) {
            for (auto otherDrawable: sprites){
                auto other = otherDrawable;
                if (sprite == other)
                    continue;
                if (sprite->getType() == ObjectType::Ship && sprite->isPointInside(other->getPos())){
                    if (other->getType() == ObjectType::Laser){
                        other->die();
                        sprite->hit();
                        //collisions.push_back(0.5f * (sprite->getPos() + other->getPos()));
                    }
               }
            }
        }

    };

}

class Game::GameImpl {
private:
    typedef std::chrono::high_resolution_clock Time;
public:

    GameImpl(std::function<void(const std::string&)> onUpdate) {
        using namespace std::chrono_literals;
        dtSp = 10ms;
        ok = false;
        state = std::make_unique<GameplayState>(onUpdate);
    }

    void update(){
        typedef std::chrono::duration<float> fsec;
        auto t1 = Time::now();
        fsec dtReal = t1 - lastTp;
        state->updateSprites(dtReal.count());
        state->makeMsg();
        lastTp = t1;
        std::this_thread::sleep_for(dtSp);
    }

    void run() {
        ok = true;
        lastTp = Time::now();
        loop_in_thread();
    }

    void accept(GameCommand& cmd){
        cmd.execute(*state);
    }

    void removePlayer(size_t playerId){
        if (state->players.find(playerId) == state->players.end())
            return;
        auto player = state->players[playerId];
        if (!player->ship.expired()){
            player->ship.lock()->die();
        }
        state->players.erase(playerId);
    }

private:
    std::unique_ptr<GameplayState> state;
    std::chrono::milliseconds dtSp;
    Time::time_point lastTp;
    std::atomic<bool> ok = false;

    void loop_in_thread() {
        try {
          auto t = std::thread([this]() {
              while(ok)
                update();
          });
          t.detach();
        }
        catch (std::exception& e) {
          std::cerr << e.what() << std::endl;
          ok = false;
        }
    }
};

Game::Game(std::function<void(const std::string&)> onUpdate): impl(new GameImpl(onUpdate)) {}

void Game::accept(GameCommand& cmd) {
    impl->accept(cmd);
}

Game::~Game() {
    delete impl;
}

void Game::run(){
    impl->run();
}

void Game::removePlayer(size_t playerId) {
    impl->removePlayer(playerId);
}
