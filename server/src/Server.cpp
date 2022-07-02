#include "Server.h"
#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <deque>
#include "Connection.h"

using boost::asio::ip::tcp;

struct tcp_server::tcp_server_impl {

   tcp_server_impl(){}

   void init(){
       io_context = std::make_unique<boost::asio::io_context>();
       acceptor = std::make_unique<tcp::acceptor>(*io_context, tcp::endpoint(tcp::v4(), 65432), true);
   }

   std::unique_ptr<boost::asio::io_context> io_context;
   std::unique_ptr<tcp::acceptor> acceptor;
   std::vector<tcp_connection::pointer> connections;
   std::deque<tcp_connection::pointer> waiting_conns;
   std::mutex mutex;

   void start(){
      init();
      start_accept();
      io_context->run();
      loop_in_thread();
   }

   void start_accept() {
       tcp_connection::pointer new_connection = tcp_connection::create(*io_context);
       acceptor->async_accept(new_connection->socket(),
           boost::bind(&tcp_server_impl::handle_accept, this, new_connection,
             boost::asio::placeholders::error));
   }

   void handle_accept(tcp_connection::pointer new_connection,
                      const boost::system::error_code& error) {
       if (!error) {
         new_connection->start();
         waiting_conns.push_back(new_connection);
       }
   }

   void check_connections(){
       while (!waiting_conns.empty()) {
           auto connection = waiting_conns.front();
           connections.push_back(connection);
           waiting_conns.pop_front();
       }
       connections.erase(std::remove_if(connections.begin(),
                               connections.end(),
                               [](tcp_connection::pointer conn_ptr){return !conn_ptr->is_alive();}),
                               connections.end());
   }

   void loop_in_thread() {
       try {
         auto t = boost::thread([this]() {
             loop();
         });
         t.detach();
       }
       catch (std::exception& e) {
         std::cerr << e.what() << std::endl;
       }
   }

   void loop() {
       mutex.lock();
       check_connections();
       mutex.unlock();
       using namespace std::chrono_literals;
       std::this_thread::sleep_for(200ms);
       loop();
   }

};

tcp_server::tcp_server(): impl(new tcp_server_impl()) {}

tcp_server::~tcp_server(){
    delete impl;
}

void tcp_server::start() {
    impl->start();
}

void tcp_server::wait() {
    while(true) {
        std::this_thread::yield();
    }
}


