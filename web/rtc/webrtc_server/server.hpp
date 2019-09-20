#ifndef SERVER_HPP
#define SERVER_HPP

#include <functional>
#include <memory>
#include "connection.hpp"

class server {
 public:
  virtual ~server();
  using async_accept_callback =
      std::function<void(std::unique_ptr< ::connection> &&)>;
  virtual void start() = 0;
  virtual void stop() = 0;
};

#endif  // SERVER_HPP
