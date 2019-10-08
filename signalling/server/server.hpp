#ifndef SERVER_SERVER_HPP
#define SERVER_SERVER_HPP

#include "logging/logger.hpp"
#include "websocket/acceptor.hpp"
#include "websocket/connection_ptr.hpp"
#include <boost/thread/executors/executor.hpp>

namespace server {
class connection;
class connection_creator;
class server {
public:
  server(boost::executor &executor, websocket::acceptor &acceptor,
         connection_creator &connection_creator_);
  ~server();
  void close();

protected:
  void on_connection(websocket::connection_ptr connection_);

  boost::executor &executor;
  websocket::acceptor &acceptor;
  connection_creator &connection_creator_;
  logging::logger logger;
};
} // namespace server

#endif
