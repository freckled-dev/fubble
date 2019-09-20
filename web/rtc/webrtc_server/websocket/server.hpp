#ifndef WEBSOCKET_SERVER_HPP
#define WEBSOCKET_SERVER_HPP

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include "../port.hpp"
#include "../server.hpp"

namespace websocket {

class server : public ::server {
 public:
  server(boost::asio::io_context& context, const port port_,
         const async_accept_callback& callback_);

  void start() override;
  void stop() override;

 private:
  void async_accept(boost::asio::yield_context yield);
  void start(boost::asio::yield_context yield);

  boost::asio::io_context& context;
  boost::asio::ip::tcp::endpoint endpoint;
  boost::asio::ip::tcp::acceptor acceptor;

  async_accept_callback callback;
};

}  // namespace websocket

#endif  // WEBSOCKET_SERVER_HPP
