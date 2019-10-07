#ifndef WEBSOCKET_CONNECTOR_HPP
#define WEBSOCKET_CONNECTOR_HPP

#include "logging/logger.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>

namespace websocket {
class connection;
class connection_creator;
class connector {
public:
  struct config {
    std::string service;
    std::string url;
    std::string path = "/";
  };
  connector(boost::asio::io_context &context, connection_creator &creator);
  std::unique_ptr<connection> operator()(const config &config_,
                                         boost::asio::yield_context yield);

private:
  logging::logger logger;
  boost::asio::ip::tcp::resolver resolver;
  connection_creator &creator;
};
} // namespace websocket

#endif
