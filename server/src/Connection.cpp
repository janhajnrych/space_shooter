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

    static const int max_length = 1024;
    char output_buffer[max_length];
    bool alive = false;
    std::shared_ptr<conrete_msg_channel> channel;
    tcp::socket tcp_socket;
    std::string input_buffer;

    tcp_connection_impl(boost::asio::io_context& io_context): tcp_socket(io_context) {
        channel = std::make_shared<conrete_msg_channel>();
    }

    void handle_read(const boost::system::error_code& err, size_t bytes_transferred) {
      if (!err) {
           channel->readQueue.push(std::string(output_buffer, bytes_transferred));
           std::cout << "read:" << output_buffer << std::endl;
      } else {
           std::cerr << "read error: " << err.value() << ":" << err.message() << std::endl;
           tcp_socket.close();
           alive = false;
      }
    }

    void handle_write(const boost::system::error_code& err, size_t) {
      if (!err) {
          if (!channel->writeQueue.empty())
              make_write();
      } else {
         std::cerr << "write error: " << err.value() << ":" << err.message() << std::endl;
         tcp_socket.close();
         alive = false;
      }
    }

    void make_read(){
        tcp_socket.async_read_some(
            boost::asio::buffer(output_buffer, max_length),
            boost::bind(&tcp_connection_impl::handle_read,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }

    void make_write() {
        if (channel->writeQueue.empty())
            return;
        input_buffer = channel->writeQueue.front();
        channel->writeQueue.pop();
        tcp_socket.async_write_some(
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

tcp_connection::tcp_connection(boost::asio::io_context& io_context): impl(new tcp_connection_impl(io_context)) {}

tcp_connection::~tcp_connection(){
    delete impl;
}

boost::asio::ip::tcp::socket& tcp_connection::socket() {
   return impl->tcp_socket;
}

void tcp_connection::start() {
    impl->alive = true;
    impl->input_buffer = "!!!";
    update();
}

void tcp_connection::update() {
    impl->make_read();
    impl->make_write();
}

bool tcp_connection::is_alive() const {
    return impl->alive;
}

std::shared_ptr<message_channel> tcp_connection::get_channel() const {
    return impl->channel;
}
