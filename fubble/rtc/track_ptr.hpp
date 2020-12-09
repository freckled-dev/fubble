#ifndef RTC_TRACK_PTR_HPP
#define RTC_TRACK_PTR_HPP

#include "track.hpp"
#include <memory>

namespace rtc {
using track_ptr = std::shared_ptr<track>;
} // namespace rtc

#endif
