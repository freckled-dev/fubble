#ifndef SIGNALLING_CLIENT_CONNECTION_CREATOR_HPP
#define SIGNALLING_CLIENT_CONNECTION_CREATOR_HPP

#include "connection_ptr.hpp"
#include "websocket/connection_ptr.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/thread/executors/executor.hpp>

namespace signalling {
class json_message;

namespace client {
class connection_creator {
public:
  connection_creator(boost::asio::io_context &context,
                     boost::executor &executor,
                     signalling::json_message &message_parser);
  connection_ptr create(websocket::connection_ptr websocket_connection);

protected:
  boost::asio::io_context &context;
  boost::executor &executor;
  signalling::json_message &message_parser;
};
} // namespace client
} // namespace signalling

#endif
