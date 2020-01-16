#ifndef WEBSOCKET_CONNECTOR_HPP
#define WEBSOCKET_CONNECTOR_HPP

#include "connection_ptr.hpp"
#include "logging/logger.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/thread/executors/executor.hpp>
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
  struct state;
  using state_ptr = std::shared_ptr<state>;
  void resolve(state_ptr &state, const config &config_);
  void connect_to_endpoints(
      state_ptr &state,
      const boost::asio::ip::tcp::resolver::results_type &results);
  void handshake(state_ptr &state, connection_ptr connection_);

  logging::logger logger;
  boost::asio::ip::tcp::resolver resolver;
  connection_creator &creator;
};
} // namespace websocket

#endif
