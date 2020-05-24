#ifndef WEBSOCKET_CONNECTION_HPP
#define WEBSOCKET_CONNECTION_HPP

#include "logger.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/thread/future.hpp>
#include <queue>

namespace websocket {
class connection {
public:
  connection(boost::asio::io_context &context, bool secure);
  ~connection();

  boost::future<void> send(const std::string &message);
  boost::future<std::string> read();
  boost::future<void> close();

  using http_stream_type =
      boost::beast::websocket::stream<boost::asio::ip::tcp::socket>;
  using https_stream_type = boost::beast::websocket::stream<
      boost::beast::ssl_stream<boost::asio::ip::tcp::socket>>;
  using stream_type = std::variant<http_stream_type, https_stream_type>;
  stream_type &get_native();
  boost::asio::ssl::context &get_ssl_context();

private:
  void send_next_from_queue();
  void on_send(const boost::system::error_code &error, std::size_t);
  void on_read(const boost::system::error_code &error, std::size_t);

  websocket::logger logger{"connection"};
  boost::asio::ssl::context ssl_context;
  stream_type stream;
  boost::beast::flat_buffer buffer;

  boost::promise<std::string> read_promise;
  struct send_item {
    std::string message;
    boost::promise<void> completion;
  };
  std::queue<send_item> send_queue;
  bool sending{};
  bool reading{};
  std::shared_ptr<int> alive_check = std::make_shared<int>(42);
};
} // namespace websocket

#endif
