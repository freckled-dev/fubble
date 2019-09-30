#ifndef SIGNALLING_DEVICE_ANSWERING_PTR_HPP
#define SIGNALLING_DEVICE_ANSWERING_PTR_HPP

#include <memory>

namespace signalling::device {
class answering;
using answering_ptr = std::shared_ptr<answering>;
using answering_wptr = std::weak_ptr<answering>;
} // namespace signalling::device

#endif
