#include "connection_creator.hpp"
#include "connection.hpp"
#include "websocket/connection.hpp"

using namespace signalling::server;

connection_creator::connection_creator(boost::asio::io_context &context,
                                       boost::executor &executor,
                                       signalling::json_message &message_parser)
    : context(context), executor(executor), message_parser(message_parser) {}
connection_creator::~connection_creator() {}

connection_ptr connection_creator::
operator()(const websocket::connection_ptr &websocket_connection) {
  auto result = std::make_shared<connection>(executor, websocket_connection,
                                             message_parser);
  return result;
}
