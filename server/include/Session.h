#ifndef SESSION_H
#define SESSION_H
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include "Channel.h"
#include <functional>
#include "Game.h"

class Session
{
public:
  Session(size_t id, std::shared_ptr<Game> game);
  ~Session();
  void accept(const std::string& msg);
private:
  struct session_impl;
  session_impl* impl;
};



#endif // SESSION_H
