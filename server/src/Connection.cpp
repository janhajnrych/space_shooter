#include "Connection.h"
#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <deque>
#include "Channel.h"
#include <optional>

using boost::asio::ip::tcp;

struct tcp_connection::tcp_connection_impl: public message_channel {
    std::shared_ptr<tcp::socket> tcp_socket;
    std::optional<tcp_connection::on_close_callback> close_callback;
    std::optional<tcp_connection::on_read_callback> read_callback;
    boost::asio::io_context& io_context;
    static const int max_length = 1024;
    size_t id;
    char output_buffer[max_length];
    char input_buffer[max_length];
    std::vector<char> heartbeat{'1', '0'};
    std::vector<std::string>::size_type counter = 0;
    bool alive = false;
    constexpr static char delimiter = ';';

    tcp_connection_impl(boost::asio::io_context& io_context, size_t id): io_context(io_context) {
        tcp_socket = std::make_shared<tcp::socket>(io_context);
        this->id = id;
        heartbeat.shrink_to_fit();
    }

    void on_close() {
        if (close_callback.has_value())
          close_callback.value()(id);
    }

    void handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
      if (!alive)
        return;
      if (!error.value()) {
           auto data = std::string(output_buffer, bytes_transferred);
           auto sepId = data.find(delimiter);
           if (sepId != std::string::npos){
               auto msg = data.substr(0, sepId);
               //std::cout << "read:" << msg << delimiter << std::endl;
               if (msg.size() > 1) {
                   read_queue.push(msg);
                   if (read_callback.has_value())
                     read_callback.value()(id);
               }
           }
      } else {
           std::cerr << "read error: " << error.value() << ":" << error.message() << std::endl;
           tcp_socket->close();
           alive = false;
           on_close();
      }
      make_write();
    }

    void handle_write(const boost::system::error_code& error, size_t) {
      if (!alive)
          return;
      if (!error.value()) {
          //auto data = std::string(input_buffer, bytes_transferred);
          //std::cout << "write:" << bytes_transferred << ":" << data << std::endl;
      } else {
         std::cerr << "write error: " << error.value() << ":" << error.message() << std::endl;
         tcp_socket->close();
         alive = false;
         on_close();
      }
      make_read();
    }

    void make_read(){
        if (!alive)
            return;
        tcp_socket->async_read_some(
            boost::asio::buffer(output_buffer, max_length),
            boost::bind(&tcp_connection_impl::handle_read,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }

    void make_write() {
        if (!alive)
            return;
        std::string msg = "";
        if (write_queue.is_empty()){
            //make_read();
            msg = std::string(1, heartbeat.at(counter)) + delimiter;
            counter = (counter + 1) % 2;
        } else {
            msg = write_queue.pull() + delimiter;
        }
        strcpy(input_buffer, msg.substr(0, max_length).c_str());

        tcp_socket->async_write_some(
            boost::asio::buffer(input_buffer, max_length),
            boost::bind(&tcp_connection_impl::handle_write,
                      this,
                      boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));

    }
};

tcp_connection::pointer tcp_connection::create(boost::asio::io_context& io_context, size_t id) {
    return tcp_connection::pointer(new tcp_connection(io_context, id));
}

tcp_connection::tcp_connection(boost::asio::io_context& io_context, size_t id):
    impl(new tcp_connection_impl(io_context, id)) {}

tcp_connection::~tcp_connection(){
    delete impl;
}

boost::asio::ip::tcp::socket& tcp_connection::socket() {
   return *impl->tcp_socket;
}

void tcp_connection::start() {
    impl->alive = true;
    impl->make_write();
}

bool tcp_connection::is_alive() const {
    return impl->alive;
}

size_t tcp_connection::getId() const {
    return impl->id;
}

void tcp_connection::on_close(std::function<void(size_t)> callback){
    impl->close_callback = callback;
}

void tcp_connection::on_read(on_read_callback callback) {
    impl->read_callback = callback;
}

void tcp_connection::write(const std::string& msg) {
    impl->write_queue.push(msg);
}

std::string tcp_connection::read() {
    return impl->read_queue.pull();
}

bool tcp_connection::has_messages() const {
    return !impl->read_queue.is_empty();
}
