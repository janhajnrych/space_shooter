#include "Client.h"
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <deque>
#include <boost/thread.hpp>
#include "Channel.h"

using boost::asio::deadline_timer;
using boost::asio::ip::tcp;

struct tcp_client::tcp_client_impl: public message_channel {

    static const int max_length = 1024;
    char output_buffer[max_length];
    char input_buffer[max_length];
    constexpr static char delimiter = ';';

    boost::asio::io_context io_context;
    std::unique_ptr<boost::asio::ip::tcp::socket> socket;

    std::vector<char> heartbeat{'1', '0'};
    std::vector<std::string>::size_type counter = 0;

    tcp_client_impl(){
        heartbeat.shrink_to_fit();
    }

    void start() {
        socket = std::make_unique<boost::asio::ip::tcp::socket>(io_context);
        boost::system::error_code error;
        socket->connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 65432), error);
        //io_context.run();
        if (error.value()){
            std::cerr << "could not connect" << std::endl;
        } else {
            boost::asio::ip::tcp::endpoint remote_ep = socket->remote_endpoint();
            boost::asio::ip::address remote_address = remote_ep.address();
            std::cout << "connected to " << remote_address.to_string() << ":" << remote_ep.port() << std::endl;
            loop_in_thread();
        }
    }

    void handle_write(const boost::system::error_code& error, size_t bytes_transferred){
        if (error.value()){
            std::cerr << "write error " << error << std::endl;
        } else {
//            auto data = std::string(input_buffer, bytes_transferred);
//            auto sepId = data.find(delimiter);
//            if (sepId != std::string::npos)
//               std::cout << "write:" << data.substr(0, sepId) << delimiter << std::endl;
        }
    }

    void handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
      if (error.value()){
            std::cerr << "read error " << error << std::endl;
      } else {
          auto data = std::string(output_buffer, bytes_transferred);
          auto sepId = data.find(delimiter);
          if (sepId != std::string::npos){
              auto msg = data.substr(0, sepId);
              read_queue.push(msg);
          }

      }
    }

    void make_write() {
        if (!socket)
            return;
        io_context.reset();
        std::string msg = "";
        if (write_queue.is_empty()){
            msg = std::string(1, heartbeat.at(counter)) + delimiter;
            counter = (counter + 1) % 2;
        } else {
            msg = write_queue.pull() + delimiter;
        }
        strcpy(input_buffer, msg.substr(0, max_length).c_str());
        socket->async_write_some(
            boost::asio::buffer(input_buffer, max_length),
            boost::bind(&tcp_client_impl::handle_write,
                      this,
                      boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));
        io_context.run();

    }

    void make_read(){
        if (!socket)
            return;
        io_context.reset();
        socket->async_read_some(
            boost::asio::buffer(output_buffer, max_length),
            boost::bind(&tcp_client_impl::handle_read,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        io_context.run();
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
        make_write();
        make_read();
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(10ms);
        loop();
    }
};

tcp_client::tcp_client(): impl(new tcp_client_impl()) {}

void tcp_client::start() {
    impl->start();
}

void tcp_client::write(const std::string& msg) {
    impl->write_queue.push(msg);
}

std::string tcp_client::read() {
    return impl->read_queue.pull();
}

bool tcp_client::has_messages() const {
    return !impl->read_queue.is_empty();
}
