#ifndef WEBSOCKET_CONNECTION_HPP
#define WEBSOCKET_CONNECTION_HPP

#include "logger.hpp"
#include <boost/thread/future.hpp>

namespace websocket {
class connection {
public:
  connection() = default;
  virtual ~connection() = default;

  virtual boost::future<void> send(const std::string &message) = 0;
  virtual boost::future<std::string> read() = 0;
  virtual boost::future<void> close() = 0;
};
} // namespace websocket

#endif
