#ifndef SIGNALLING_DEVICE_OFFERING_PTR_HPP
#define SIGNALLING_DEVICE_OFFERING_PTR_HPP

#include <memory>

namespace signalling::device {
class offering;
using offering_ptr = std::shared_ptr<offering>;
} // namespace signalling::device

#endif
