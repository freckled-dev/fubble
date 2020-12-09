#include "connection_creator.hpp"
#include "connection.hpp"
#include "websocket/connection.hpp"

using namespace signaling::client;

connection_creator::connection_creator(boost::asio::io_context &context,
                                       boost::executor &executor,
                                       signaling::json_message &message_parser)
    : context(context), executor(executor), message_parser(message_parser) {}

connection_ptr
connection_creator::create(websocket::connection_ptr websocket_connection) {
  return std::make_unique<connection>(executor, std::move(websocket_connection),
                                      message_parser);
}
