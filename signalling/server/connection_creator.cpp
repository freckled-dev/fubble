#include "connection_creator.hpp"
#include "websocket/connection.hpp"

using namespace server;

connection_creator::connection_creator(signalling::json_message &message_parser)
    : message_parser(message_parser) {}
connection_creator::~connection_creator() {}

std::unique_ptr<connection> connection_creator::
operator()(websocket::connection &websocket_connection) {
  auto result =
      std::make_unique<connection>(websocket_connection, message_parser);
  return result;
}
