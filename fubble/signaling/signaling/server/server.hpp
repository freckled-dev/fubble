#ifndef SERVER_SERVER_HPP
#define SERVER_SERVER_HPP

#include "fubble/websocket/connection_ptr.hpp"
#include "signaling/logger.hpp"
#include <boost/thread/executors/executor.hpp>

namespace websocket {
class acceptor;
}
namespace signaling {
class registration_handler;

namespace server {
class connection;
class connection_creator;
class server {
public:
  server(boost::executor &executor, websocket::acceptor &acceptor,
         connection_creator &connection_creator_,
         signaling::registration_handler &registration_handler);
  ~server();
  void close();
  int get_port() const;

protected:
  void on_connection(websocket::connection_ptr connection_);

  boost::executor &executor;
  websocket::acceptor &acceptor;
  connection_creator &connection_creator_;
  signaling::registration_handler &registration_handler;
  signaling::logger logger{"server"};
};
} // namespace server
} // namespace signaling

#endif
