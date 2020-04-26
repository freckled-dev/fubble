#ifndef RTC_TRACK_PTR_HPP
#define RTC_TRACK_PTR_HPP

#include <memory>
#include "track.hpp"

namespace rtc {
//class track;
using track_ptr = std::shared_ptr<track>;
} // namespace rtc

#endif
