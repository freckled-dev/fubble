#ifndef SIGNALLING_CLIENT_CLIENT_HPP
#define SIGNALLING_CLIENT_CLIENT_HPP

#include "connection_ptr.hpp"
#include "logging/logger.hpp"
#include "websocket/connector.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/future.hpp>

namespace client {
class connection_creator;
class connection;
class client {
public:
  client(boost::executor &executor, websocket::connector &connector,
         connection_creator &connection_creator_);

  void close();
  boost::signals2::signal<void()> on_connected;
  boost::signals2::signal<void()> on_create_offer;
  boost::signals2::signal<void(const boost::system::system_error &)> on_error;

  void operator()(const std::string &host, const std::string &service,
                  const std::string &key);
  connection_ptr get_connection() const;

private:
  void connected(boost::future<websocket::connection_ptr> &result,
                 const std::string &key);
  void connect_signals(const connection_ptr &connection_) const;
  void run_done(boost::future<void> &result);

  logging::logger logger;
  boost::executor &executor;
  websocket::connector &connector;
  connection_creator &connection_creator_;

  connection_ptr connection_;
};
} // namespace client

#endif
