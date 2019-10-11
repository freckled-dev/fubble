#ifndef WEBSOCKET_CONNECTION_HPP
#define WEBSOCKET_CONNECTION_HPP

#include "logging/logger.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/thread/future.hpp>
#include <queue>

namespace websocket {
class connection {
public:
  connection(boost::asio::io_context &context);

  boost::future<void> send(const std::string &message);
  boost::future<std::string> read();
  boost::future<void> close();

  using stream_type =
      boost::beast::websocket::stream<boost::asio::ip::tcp::socket>;
  stream_type &get_native();

private:
  void send_next_from_queue();
  void on_send(const boost::system::error_code &error, std::size_t);

  logging::logger logger;
  boost::asio::io_context &context;
  stream_type stream;
  boost::beast::flat_buffer buffer;

  struct send_item {
    std::string message;
    boost::promise<void> completion;
  };
  std::queue<send_item> send_queue;
  bool sending{};
};
} // namespace websocket

#endif
