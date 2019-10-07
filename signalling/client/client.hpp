#ifndef SIGNALLING_CLIENT_CLIENT_HPP
#define SIGNALLING_CLIENT_CLIENT_HPP

#include "connection_ptr.hpp"
#include "logging/logger.hpp"
#include "websocket/connector.hpp"
#include <boost/asio/spawn.hpp>
#include <boost/thread/future.hpp>

namespace client {
class connection_creator;
class connection;
class client {
public:
  client(boost::asio::io_context &context, websocket::connector &connector,
         connection_creator &connection_creator_);

  [[nodiscard]] boost::future<connection_ptr>
  operator()(const std::string &host, const std::string &service);

private:
  connection_ptr connect(const websocket::connector::config &config,
                         boost::asio::yield_context yield);

  logging::logger logger;
  boost::asio::io_context &context;
  websocket::connector &connector;
  connection_creator &connection_creator_;

  connection_ptr connection_;
};
} // namespace client

#endif
