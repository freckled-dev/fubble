#ifndef SIGNALING_DEVICE_CREATOR_HPP
#define SIGNALING_DEVICE_CREATOR_HPP

#include "device_ptr.hpp"
#include "signaling/connection_ptr.hpp"
#include <boost/thread/executors/executor.hpp>

namespace signaling::device {
class creator {
public:
  // TODO remove executor. it's unused
  creator(boost::executor &executor);
  virtual ~creator() = default;
  virtual device_ptr create(connection_ptr connection_,
                            const std::string &token);

protected:
  boost::executor &executor;
};
} // namespace signaling::device

#endif
