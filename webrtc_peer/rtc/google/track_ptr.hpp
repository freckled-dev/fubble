#ifndef RTC_GOOGLE_TRACK_PTR_HPP
#define RTC_GOOGLE_TRACK_PTR_HPP

#include <memory>
#include "track.hpp"

namespace rtc::google {
using track_ptr = std::shared_ptr<track>;
} // namespace rtc::google

#endif
