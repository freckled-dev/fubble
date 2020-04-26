#ifndef WEBSOCKET_CONNECTOR_HPP
#define WEBSOCKET_CONNECTOR_HPP

#include "connection_ptr.hpp"
#include "connection.hpp"
#include "logger.hpp"
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
  connector(boost::asio::io_context &context, connection_creator &creator,
            const config &config_);
  ~connector();

  using future_type = boost::future<connection_ptr>;
  future_type connect();

private:
  void resolve();
  void on_resolved(const boost::system::error_code &error,
                   const boost::asio::ip::tcp::resolver::results_type &results);
  void connect_to_endpoints(
      const boost::asio::ip::tcp::resolver::results_type &results);
  void on_connected(const boost::system::error_code &error);
  void handshake();
  bool check_error(const boost::system::error_code &error);

  websocket::logger logger{"connector"};
  connection_creator &creator;
  boost::asio::ip::tcp::resolver resolver;
  boost::promise<connection_ptr> promise;
  const connector::config config_;
  connection_ptr connection;
  bool done{};
};

// TODO move to own file
class connector_creator {
public:
  connector_creator(boost::asio::io_context &context,
                    connection_creator &creator);
  ~connector_creator();

  std::unique_ptr<connector> create(const connector::config &config_);

private:
  websocket::logger logger{"connector_creator"};
  boost::asio::io_context &context;
  connection_creator &creator;
};
} // namespace websocket

#endif
