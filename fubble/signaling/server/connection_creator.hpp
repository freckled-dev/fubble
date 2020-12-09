#ifndef SERVER_CONNECTION_CREATOR_HPP
#define SERVER_CONNECTION_CREATOR_HPP

#include "connection_ptr.hpp"
#include "fubble/websocket/connection_ptr.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/thread/executors/executor.hpp>

namespace signaling {
class json_message;

namespace server {
class connection_creator {
public:
  connection_creator(boost::executor &executor,
                     signaling::json_message &message_parser);
  virtual ~connection_creator();

  connection_ptr create(websocket::connection_ptr websocket_connection);

private:
  boost::executor &executor;
  signaling::json_message &message_parser;
};
} // namespace server
} // namespace signaling

#endif
