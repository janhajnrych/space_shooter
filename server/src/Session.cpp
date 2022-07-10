#include "Session.h"
#include "CommandFactory.h"

struct Session::session_impl {
    size_t id;
    std::unique_ptr<ServerCmdFactory> cmdFactory;
    std::shared_ptr<Game> game;
};

Session::Session(size_t id, std::shared_ptr<Game> game): impl(new session_impl()){
    impl->id = id;
    impl->game = game;
    impl->cmdFactory = std::make_unique<ServerCmdFactory>(id);
}

Session::~Session() {
    delete impl;
}

void Session::accept(const std::string& msg) {
    auto cmd = impl->cmdFactory->create(msg);
    if (cmd)
        impl->game->accept(*cmd);
}
