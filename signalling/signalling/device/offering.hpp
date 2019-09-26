#ifndef SIGNALLING_DEVICE_OFFERING_HPP
#define SIGNALLING_DEVICE_OFFERING_HPP

#include "signalling/connection_ptr.hpp"

namespace signalling::device {
class offering {
public:
  offering(connection_ptr connection_);
};
} // namespace signalling::device

#endif

