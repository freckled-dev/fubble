#include "connection_creator.hpp"
#include "connection.hpp"
#include "websocket/connection.hpp"

using namespace server;

connection_creator::connection_creator(signalling::json_message &message_parser)
    : message_parser(message_parser) {}
connection_creator::~connection_creator() {}

connection_ptr connection_creator::
operator()(const websocket::connection_ptr &websocket_connection) {
  auto result =
      std::make_shared<connection>(websocket_connection, message_parser);
  return result;
}
