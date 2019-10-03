#ifndef SERVER_CONNECTION_CREATOR_HPP
#define SERVER_CONNECTION_CREATOR_HPP

#include "connection.hpp"

namespace websocket {
class connection;
}
namespace signalling {
class json_message;
}

namespace server {
class connection_creator {
public:
  connection_creator(signalling::json_message &message_parser);
  virtual ~connection_creator();

  std::unique_ptr<connection>
  operator()(websocket::connection &websocket_connection);

private:
  signalling::json_message &message_parser;
};
} // namespace server

#endif
