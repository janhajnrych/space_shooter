#ifndef CONNECTION_H
#define CONNECTION_H
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include "Channel.h"

class tcp_connection
{
public:
  typedef boost::shared_ptr<tcp_connection> pointer;
  virtual ~tcp_connection();
  static pointer create(boost::asio::io_context& io_context);
  static pointer create(std::shared_ptr<boost::asio::ip::tcp::socket> tcp_socket);
  boost::asio::ip::tcp::socket& socket();
  void start();
  bool is_alive() const;
private:
  tcp_connection(boost::asio::io_context& io_context);
  tcp_connection(std::shared_ptr<boost::asio::ip::tcp::socket> tcp_socket);
  struct tcp_connection_impl;
  tcp_connection_impl* impl;
};



#endif // CONNECTION_H
