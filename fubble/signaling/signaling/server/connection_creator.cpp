#include "connection_creator.hpp"
#include "connection.hpp"
#include "fubble/websocket/connection.hpp"

using namespace signaling::server;

connection_creator::connection_creator(boost::executor &executor,
                                       signaling::json_message &message_parser)
    : executor(executor), message_parser(message_parser) {}
connection_creator::~connection_creator() {}

connection_ptr
connection_creator::create(websocket::connection_ptr websocket_connection) {
  auto result = std::make_shared<connection>(
      executor, std::move(websocket_connection), message_parser);
  return result;
}
