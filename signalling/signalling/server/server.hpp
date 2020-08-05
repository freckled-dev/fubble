#ifndef SERVER_SERVER_HPP
#define SERVER_SERVER_HPP

#include "signalling/logger.hpp"
#include "websocket/connection_ptr.hpp"
#include <boost/thread/executors/executor.hpp>

namespace websocket {
class acceptor;
}
namespace signalling {
class registration_handler;

namespace server {
class connection;
class connection_creator;
class server {
public:
  server(boost::executor &executor, websocket::acceptor &acceptor,
         connection_creator &connection_creator_,
         signalling::registration_handler &registration_handler);
  ~server();
  void close();
  int get_port() const;

protected:
  void on_connection(websocket::connection_ptr connection_);

  boost::executor &executor;
  websocket::acceptor &acceptor;
  connection_creator &connection_creator_;
  signalling::registration_handler &registration_handler;
  signalling::logger logger{"server"};
};
} // namespace server
} // namespace signalling

#endif
