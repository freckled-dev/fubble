#ifndef WEBSOCKET_ACCEPTOR_HPP
#define WEBSOCKET_ACCEPTOR_HPP

#include "connection_ptr.hpp"
#include "logging/logger.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/signals2/signal.hpp>

namespace websocket {
class connection_creator;
class acceptor {
public:
  struct config {
    std::uint16_t port{};
  };
  acceptor(boost::asio::io_context &context, connection_creator &creator,
           const config &config_);
  void close();
  std::uint16_t get_port() const;
  boost::signals2::signal<void(connection_ptr &)> on_connection;

private:
  void run();
  void successful_tcp(connection_ptr connection_);

  logging::logger logger;
  boost::asio::ip::tcp::acceptor acceptor_;
  connection_creator &creator;
};
} // namespace websocket

#endif
