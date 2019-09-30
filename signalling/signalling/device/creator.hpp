#ifndef SIGNALLING_DEVICE_CREATOR_HPP
#define SIGNALLING_DEVICE_CREATOR_HPP

#include "answering_ptr.hpp"
#include "offering_ptr.hpp"
#include "signalling/connection_ptr.hpp"
#include <boost/thread/executors/generic_executor_ref.hpp>

namespace signalling::device {
class creator {
public:
  creator(boost::generic_executor_ref &executor);
  virtual ~creator() = default;
  virtual offering_ptr create_offering(connection_ptr connection_);
  virtual answering_ptr create_answering(connection_ptr connection_);

protected:
  boost::generic_executor_ref executor;
};
} // namespace signalling::device

#endif
