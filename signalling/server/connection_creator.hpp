#ifndef SERVER_CONNECTION_CREATOR_HPP
#define SERVER_CONNECTION_CREATOR_HPP

#include "connection_ptr.hpp"
#include "websocket/connection_ptr.hpp"

namespace signalling {
class json_message;
}

namespace server {
class connection_creator {
public:
  connection_creator(signalling::json_message &message_parser);
  virtual ~connection_creator();

  connection_ptr
  operator()(const websocket::connection_ptr &websocket_connection);

private:
  signalling::json_message &message_parser;
};
} // namespace server

#endif
