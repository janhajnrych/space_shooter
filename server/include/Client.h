#ifndef CLIENT_H
#define CLIENT_H
#include <functional>

class tcp_client
{
public:
  tcp_client();
  void start();
  void write(const std::string& msg);
  std::string read();
  bool has_messages() const;
private:
  struct tcp_client_impl;
  tcp_client_impl* impl;
};



#endif // CLIENT_H
