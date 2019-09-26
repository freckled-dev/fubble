#ifndef SIGNALLING_DEVICE_CREATOR_HPP
#define SIGNALLING_DEVICE_CREATOR_HPP

#include "signalling/connection_ptr.hpp"

namespace signalling::device {
class offering;
class answering;
class creator {
public:
  virtual ~creator() = default;
  virtual std::unique_ptr<offering> create_offering(connection_ptr connection_);
  virtual std::unique_ptr<answering>
  create_answering(connection_ptr connection_);
};
} // namespace signalling::device

#endif
