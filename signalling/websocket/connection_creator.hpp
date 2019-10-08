#ifndef WEBSOCKET_CONNECTION_CREATOR_HPP
#define WEBSOCKET_CONNECTION_CREATOR_HPP

#include "connection_ptr.hpp"
#include <boost/asio/io_context.hpp>

namespace websocket {
class connection_creator {
public:
  connection_creator(boost::asio::io_context &context);
  virtual ~connection_creator() = default;

  virtual connection_ptr operator()();

private:
  boost::asio::io_context &context;
};
} // namespace websocket

#endif
