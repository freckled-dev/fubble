#ifndef WEBSOCKET_CONNECTION_HPP
#define WEBSOCKET_CONNECTION_HPP

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/beast/websocket/stream.hpp>

namespace websocket {
class connection {
public:
  connection(boost::asio::io_context &context);

  void send(boost::asio::yield_context yield, std::string_view message);
  std::function<void(std::string_view)> on_message;

  using stream_type =
      boost::beast::websocket::stream<boost::asio::ip::tcp::socket>;
  stream_type &get_native();

private:
  stream_type stream;
};
} // namespace websocket

#endif

