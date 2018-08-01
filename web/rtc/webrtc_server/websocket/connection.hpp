#ifndef WEBSOCKET_CONNECTION_HPP
#define WEBSOCKET_CONNECTION_HPP

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/websocket/stream.hpp>
#include "../connection.hpp"

namespace websocket {

class connection : public ::connection {
 public:
  connection(boost::asio::io_context &context);
  ~connection();

  boost::asio::ip::tcp::socket socket;
  boost::beast::websocket::stream<boost::asio::ip::tcp::socket &> stream{
      socket};
};

}  // namespace websocket

#endif  // WEBSOCKET_CONNECTION_HPP
