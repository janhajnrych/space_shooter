#ifndef SERVER_H
#define SERVER_H
#include <deque>
#include <string>
#include <functional>
#include <memory>

class message_channel;

class tcp_server
{
public:
  tcp_server();
  ~tcp_server();
  void start();
  void update();
  using channel_callback = std::function<void(std::shared_ptr<message_channel>)>;
  void on_new_connection(channel_callback callback);

private:
  struct tcp_server_impl;
  tcp_server_impl* impl;
};



#endif // SERVER_H
