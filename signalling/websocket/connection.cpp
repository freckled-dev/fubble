#include "connection.hpp"
#include <boost/beast/core/buffers_to_string.hpp>

using namespace websocket;

connection::connection(boost::asio::io_context &context) : stream(context) {}

connection::stream_type &connection::get_native() { return stream; }

void connection::send(boost::asio::yield_context yield,
                      std::string_view message) {
  stream.async_write(boost::asio::buffer(message), yield);
}

std::string connection::read(boost::asio::yield_context yield) {
  stream.async_read(buffer, yield);
  std::string result = boost::beast::buffers_to_string(buffer.data());
  buffer.consume(buffer.size());
  return result;
}
