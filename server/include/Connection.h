#ifndef CONNECTION_H
#define CONNECTION_H
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include "Channel.h"
#include <functional>

class tcp_connection
{
public:
  typedef boost::shared_ptr<tcp_connection> pointer;
  typedef std::function<void(size_t)> on_close_callback;
  typedef std::function<void(size_t)> on_read_callback;
  virtual ~tcp_connection();
  static pointer create(boost::asio::io_context& io_context, size_t id);
  //static pointer create(std::shared_ptr<boost::asio::ip::tcp::socket> tcp_socket);
  boost::asio::ip::tcp::socket& socket();
  void start();
  bool is_alive() const;
  void on_close(on_close_callback callback);
  void on_read(on_read_callback callback);
  size_t getId() const;
  void write(const std::string& msg);
  std::string read();
  bool has_messages() const;
private:
  tcp_connection(boost::asio::io_context& io_context, size_t id);
  //tcp_connection(std::shared_ptr<boost::asio::ip::tcp::socket> tcp_socket);
  struct tcp_connection_impl;
  tcp_connection_impl* impl;
};



#endif // CONNECTION_H
