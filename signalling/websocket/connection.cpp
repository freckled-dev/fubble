#include "connection.hpp"
#include <boost/beast/core/buffers_to_string.hpp>

using namespace websocket;

connection::connection(boost::asio::io_context &context) : stream(context) {}

connection::stream_type &connection::get_native() { return stream; }

void connection::send(boost::asio::yield_context yield,
                      std::string_view message) {
  stream.async_write(boost::asio::buffer(message), yield);
}

void connection::close(boost::asio::yield_context yield) {
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "closing websocket connection";
  stream.async_close(boost::beast::websocket::close_code::normal, yield);
}

std::string connection::read(boost::asio::yield_context yield) {
  stream.async_read(buffer, yield);
  std::string result = boost::beast::buffers_to_string(buffer.data());
  buffer.consume(buffer.size());
  return result;
}
