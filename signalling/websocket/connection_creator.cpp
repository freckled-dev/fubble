#include "connection_creator.hpp"
#include "connection.hpp"

using namespace websocket;

connection_creator::connection_creator(boost::asio::io_context &context)
    : context(context) {}

connection_ptr connection_creator::operator()() {
  auto result = std::make_unique<connection>(context);
  return result;
}