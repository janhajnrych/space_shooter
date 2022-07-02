#include "Connection.h"
#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <deque>

using boost::asio::ip::tcp;

namespace  {

    template <class T>
    class restricted_queue : protected std::deque<T> {
    public:

        T front() const {
            return std::deque<T>::front();
        }

        void pop(){
          const std::lock_guard<std::mutex> lock(mutex);
          std::deque<T>::pop_front();
        }

        bool empty() const {
            return std::deque<T>::empty();
        }

        void push(const T& elem) {
            const std::lock_guard<std::mutex> lock(mutex);
            std::deque<T>::push_back(elem);
        }

    private:
        std::mutex mutex;
    };

    class conrete_msg_channel: public message_channel {
    public:

        restricted_queue<std::string> writeQueue;
        restricted_queue<std::string> readQueue;

        virtual ~conrete_msg_channel() = default;

        std::string read() override {
            const auto msg = readQueue.front();
            std::cout << msg << std::endl;
            readQueue.pop();
            return msg;
        }

        bool has_data_to_read() const override {
            return !readQueue.empty();
        }

        void write(const std::string& msg) override {
            writeQueue.push(msg);
        }

    };

}

struct tcp_connection::tcp_connection_impl {
    std::shared_ptr<tcp::socket> tcp_socket;
    static const int max_length = 1024;
    char output_buffer[max_length];
    char input_buffer[max_length];
    bool alive = false;
    std::vector<std::string> pingpong{"server_ping", "server_pong"};
    std::vector<std::string>::size_type counter = 0;

    tcp_connection_impl(boost::asio::io_context& io_context) {
        tcp_socket = std::make_shared<tcp::socket>(io_context);
    }

    tcp_connection_impl(std::shared_ptr<tcp::socket> tcp_socket) {
        this->tcp_socket = tcp_socket;
    }

    void handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
      if (!error.value()) {
           auto data = std::string(output_buffer, bytes_transferred);
           std::cout << "read:" << data << std::endl;
      } else {
           std::cerr << "read error: " << error.value() << ":" << error.message() << std::endl;
           tcp_socket->close();
           alive = false;
      }
      make_write();
    }

    void handle_write(const boost::system::error_code& error, size_t bytes_transferred) {
      if (!error.value()) {
          auto data = std::string(input_buffer, bytes_transferred);
          std::cout << "write:" << data << std::endl;
      } else {
         std::cerr << "write error: " << error.value() << ":" << error.message() << std::endl;
         tcp_socket->close();
         alive = false;
      }
      make_read();
    }

    void make_read(){
        tcp_socket->async_read_some(
            boost::asio::buffer(output_buffer, max_length),
            boost::bind(&tcp_connection_impl::handle_read,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }

    void make_write() {
        std::string msg = pingpong.at(counter);
        counter = (counter + 1) % 2;
        strcpy(input_buffer, msg.substr(0, max_length).c_str());
        tcp_socket->async_write_some(
            boost::asio::buffer(input_buffer, max_length),
            boost::bind(&tcp_connection_impl::handle_write,
                      this,
                      boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));

    }
};

tcp_connection::pointer tcp_connection::create(boost::asio::io_context& io_context) {
    return tcp_connection::pointer(new tcp_connection(io_context));
}

tcp_connection::pointer tcp_connection::create(std::shared_ptr<boost::asio::ip::tcp::socket> tcp_socket) {
    return tcp_connection::pointer(new tcp_connection(tcp_socket));
}

tcp_connection::tcp_connection(boost::asio::io_context& io_context):
    impl(new tcp_connection_impl(io_context)) {}

tcp_connection::tcp_connection(std::shared_ptr<boost::asio::ip::tcp::socket> tcp_socket):
    impl(new tcp_connection_impl(tcp_socket)) {}

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
