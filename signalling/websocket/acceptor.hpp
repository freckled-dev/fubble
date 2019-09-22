#ifndef WEBSOCKET_ACCEPTOR_HPP
#define WEBSOCKET_ACCEPTOR_HPP

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>

namespace websocket {
class connection;
class connection_creator;
class acceptor {
public:
  struct config {
    std::uint16_t port{};
  };
  acceptor(boost::asio::io_context &context, connection_creator &creator,
           const config &config_);
  std::unique_ptr<connection> operator()(boost::asio::yield_context yield);
  std::uint16_t get_port() const;

private:
  boost::asio::ip::tcp::acceptor acceptor_;
  connection_creator &creator;
};
} // namespace websocket

#endif
