#ifndef RTC_GOOGLE_VIDEO_TRACK_PTR_HPP
#define RTC_GOOGLE_VIDEO_TRACK_PTR_HPP

#include <memory>

namespace rtc::google {
class video_track;
using video_track_ptr = std::shared_ptr<video_track>;
} // namespace rtc::google

#endif
