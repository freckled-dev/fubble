#ifndef SIGNALLING_DEVICE_ANSWERING_HPP
#define SIGNALLING_DEVICE_ANSWERING_HPP

#include "signalling/connection_ptr.hpp"
#include <boost/thread/executors/generic_executor_ref.hpp>

namespace signalling::device {
class answering {
public:
  answering(boost::generic_executor_ref &executor, connection_ptr connection_);
};
} // namespace signalling::device

#endif
