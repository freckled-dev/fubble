#ifndef WEBSOCKET_CONNECTION_HPP
#define WEBSOCKET_CONNECTION_HPP

#include "logging/logger.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/beast/websocket/stream.hpp>

namespace websocket {
class connection {
public:
  connection(boost::asio::io_context &context);

  void send(boost::asio::yield_context yield, std::string_view message);
  std::string read(boost::asio::yield_context yield);
  void close(boost::asio::yield_context yield);

  using stream_type =
      boost::beast::websocket::stream<boost::asio::ip::tcp::socket>;
  stream_type &get_native();

private:
  logging::logger logger;
  stream_type stream;
  boost::beast::flat_buffer buffer;
};
} // namespace websocket

#endif
