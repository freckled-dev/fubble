#ifndef SIGNALLING_CONNECTION_HPP
#define SIGNALLING_CONNECTION_HPP

#include "registration.hpp"
#include <boost/thread/future.hpp>
#include <functional>

namespace signalling {
class connection {
public:
  virtual ~connection() = default;

  virtual boost::future<registration> read_registration() = 0;
};
} // namespace signalling

#endif
