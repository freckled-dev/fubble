#ifndef SERVER_SERVER_HPP
#define SERVER_SERVER_HPP

#include "logging/logger.hpp"
#include "websocket/acceptor.hpp"
#include "websocket/connection_ptr.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/asio/spawn.hpp>

namespace server {
class connection;
class connection_creator;
class server {
public:
  server(boost::asio::io_context &context, websocket::acceptor &acceptor,
         connection_creator &connection_creator_);

  void run();
  void run(boost::asio::yield_context context);
  void close();

protected:
  void handle_connection(const websocket::connection_ptr &connection,
                         boost::asio::yield_context yield);
  void handle_connection(connection &connection_,
                         boost::asio::yield_context yield);

  boost::asio::io_context &context;
  websocket::acceptor &acceptor;
  connection_creator &connection_creator_;
  logging::logger logger;
};
} // namespace server

#endif
