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
       acceptor = std::make_unique<tcp::acceptor>(*io_context, tcp::endpoint(tcp::v4(), 65432));
   }

   std::unique_ptr<boost::asio::io_context> io_context;
   std::unique_ptr<tcp::acceptor> acceptor;
   std::vector<tcp_connection::pointer> connections;
   std::deque<tcp_connection::pointer> waiting_conns;
   std::mutex mutex;
   std::optional<channel_callback> on_new_connection_callback;

   void run(){
      init();
      start_accept();
      io_context->run();
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
       start_accept();
   }

};

tcp_server::tcp_server(): impl(new tcp_server_impl()) {}

tcp_server::~tcp_server(){
    delete impl;
}

void tcp_server::start() {
  try {
    auto t = boost::thread([this]() {
        impl->run();
    });
    t.detach();
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}

void tcp_server::update() {
    const std::lock_guard<std::mutex> lock(impl->mutex);
    while (!impl->waiting_conns.empty()){
        auto connection = impl->waiting_conns.front();
        impl->connections.push_back(connection);
        impl->waiting_conns.pop_front();
        if (impl->on_new_connection_callback.has_value()){
            auto channel = connection->get_channel();
            impl->on_new_connection_callback.value()(channel);
        }
    }
    impl->connections.erase(std::remove_if(impl->connections.begin(),
                            impl->connections.end(),
                            [](tcp_connection::pointer conn_ptr){return !conn_ptr->is_alive();}),
                            impl->connections.end());
    for (auto conn : impl->connections){
        conn->update();
    }
}

void tcp_server::on_new_connection(channel_callback callback) {
    impl->on_new_connection_callback = callback;
}



