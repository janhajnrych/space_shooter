#include "Server.h"
#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <list>
#include "Connection.h"
#include "Session.h"
#include "Game.h"
#include "Messages.h"

using boost::asio::ip::tcp;

struct tcp_server::tcp_server_impl {

   tcp_server_impl() {
       io_context = std::make_unique<boost::asio::io_context>();
       acceptor = std::make_unique<tcp::acceptor>(*io_context, tcp::endpoint(tcp::v4(), 65432), true);
       game = std::make_shared<Game>(std::bind(&tcp_server_impl::on_message, this, std::placeholders::_1));
   }

   std::unique_ptr<boost::asio::io_context> io_context;
   std::unique_ptr<tcp::acceptor> acceptor;
   std::unordered_map<size_t, tcp_connection::pointer> connections;
   std::unordered_map<size_t, std::unique_ptr<Session>> sessions;
   size_t nextId = 0;
   std::shared_ptr<Game> game;
   bool ok = true;

   void start(){
      start_accept();
      game->run();
      io_context->run();
   }

   void start_accept() {
       tcp_connection::pointer new_connection = tcp_connection::create(*io_context, nextId);
       nextId++;
       new_connection->on_close(std::bind(&tcp_server_impl::remove_dead_connection, this, std::placeholders::_1));
       new_connection->on_read(std::bind(&tcp_server_impl::on_read, this, std::placeholders::_1));
       acceptor->async_accept(new_connection->socket(),
           boost::bind(&tcp_server_impl::handle_accept, this, new_connection,
             boost::asio::placeholders::error));
   }

   void on_read(size_t id){
        auto connection = connections[id];
        auto& session = sessions[id];
        while (connection->has_messages()){
            auto msg = connection->read();
            session->accept(msg);
        }
   }

   void on_message(const std::string& msg){
       std::cout << "state: " << msg << std::endl;
       for (auto pair: connections){
           pair.second->write(msg);
       }
   }

   void handle_accept(tcp_connection::pointer new_connection,
                      const boost::system::error_code& error) {
       if (!error) {
         std::cout << "new connection: id=" << new_connection->getId() << std::endl;
         new_connection->start();
         auto id = new_connection->getId();
         connections[id] = new_connection;
         sessions[id] = std::make_unique<Session>(id, game);
       } else {
            std::cerr << error.message() << std::endl;
       }
       start_accept();
   }

   void remove_dead_connection(size_t id){
       game->removePlayer(id);
       connections.erase(id);
       sessions.erase(id);
       std::cout << "n conn = " << connections.size() << std::endl;
   }

};

tcp_server::tcp_server(): impl(new tcp_server_impl()) {}

tcp_server::~tcp_server(){
    delete impl;
}

void tcp_server::start() {
    impl->start();
}


