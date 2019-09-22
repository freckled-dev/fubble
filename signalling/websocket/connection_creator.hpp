#ifndef WEBSOCKET_CONNECTION_CREATOR_HPP
#define WEBSOCKET_CONNECTION_CREATOR_HPP

#include <boost/asio/io_context.hpp>

namespace websocket {
class connection;

class connection_creator {
public:
  connection_creator(boost::asio::io_context &context);
  virtual ~connection_creator() = default;

  virtual std::unique_ptr<connection> operator()();

private:
  boost::asio::io_context &context;
};
} // namespace websocket

#endif

