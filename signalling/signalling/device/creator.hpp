#ifndef SIGNALLING_DEVICE_CREATOR_HPP
#define SIGNALLING_DEVICE_CREATOR_HPP

#include "device_ptr.hpp"
#include "signalling/connection_ptr.hpp"
#include <boost/thread/executors/executor.hpp>

namespace signalling::device {
class creator {
public:
  // TODO remove executor. it's unused
  creator(boost::executor &executor);
  virtual ~creator() = default;
  virtual device_ptr create(connection_ptr connection_);

protected:
  boost::executor &executor;
};
} // namespace signalling::device

#endif
