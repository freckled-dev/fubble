#include "connection_creator.hpp"
#include "connection.hpp"

using namespace client;

connection_creator::connection_creator(boost::asio::io_context &context,
                                       signalling::json_message &message_parser)
    : context(context), message_parser(message_parser) {}

connection_ptr connection_creator::
operator()(const websocket::connection_ptr &websocket_connection) {
  auto result = std::make_shared<connection>(context, websocket_connection,
                                             message_parser);
  return result;
}
