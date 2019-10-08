#ifndef WEBSOCKET_CONNECTION_HPP
#define WEBSOCKET_CONNECTION_HPP

#include "logging/logger.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/thread/future.hpp>

namespace websocket {
class connection {
public:
  connection(boost::asio::io_context &context);

  boost::future<void> send(std::string_view message);
  boost::future<std::string> read();
  boost::future<void> close();

  using stream_type =
      boost::beast::websocket::stream<boost::asio::ip::tcp::socket>;
  stream_type &get_native();

private:
  logging::logger logger;
  boost::asio::io_context &context;
  stream_type stream;
  boost::beast::flat_buffer buffer;
};
} // namespace websocket

#endif
