#include "connection_creator.hpp"
#include "connection_impl.hpp"

using namespace websocket;

connection_creator::connection_creator(boost::asio::io_context &context)
    : context(context) {}

connection_ptr connection_creator::create(bool secure) {
  auto result = std::make_unique<connection_impl>(context, secure);
  return result;
}
