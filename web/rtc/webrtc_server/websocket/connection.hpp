#ifndef WEBSOCKET_CONNECTION_HPP
#define WEBSOCKET_CONNECTION_HPP

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/beast/websocket/stream.hpp>
#include "../connection.hpp"

namespace websocket {

class connection : public ::connection {
 public:
  boost::asio::ip::tcp::socket socket;
  boost::beast::websocket::stream<boost::asio::ip::tcp::socket &> stream{
      socket};
  callback_message_type callback_message;

  connection(boost::asio::io_context &context);
  ~connection() override;

  void read_async(const callback_message_type &callback_message) override;
  void write(const message_type &message) override;

 private:
  void read_async(boost::asio::yield_context yield);
};

}  // namespace websocket

#endif  // WEBSOCKET_CONNECTION_HPP
