#ifndef WEBSOCKET_CONNECTOR_HPP
#define WEBSOCKET_CONNECTOR_HPP

#include "connection_ptr.hpp"
#include "logging/logger.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/thread/future.hpp>

namespace websocket {
class connection_creator;
class connector {
public:
  struct config {
    std::string service;
    std::string url;
    std::string path = "/";
  };
  connector(boost::asio::io_context &context, connection_creator &creator);
  using future_type = boost::future<connection_ptr>;
  future_type operator()(const config &config_);

private:
  using promise_type = boost::promise<connection_ptr>;
  boost::future<boost::asio::ip::tcp::resolver::results_type>
  resolve(const config &config_);
  future_type connect_to_endpoints(
      const boost::asio::ip::tcp::resolver::results_type &results);
  future_type handshake(connection_ptr connection_, const config &config_);

  logging::logger logger;
  boost::asio::io_context &context;
  boost::asio::ip::tcp::resolver resolver;
  connection_creator &creator;
};
} // namespace websocket

#endif
