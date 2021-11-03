#ifndef RTC_GOOGLE_VIDEO_TRACK_HPP
#define RTC_GOOGLE_VIDEO_TRACK_HPP

#include <fubble/rtc/google/track.hpp>
#include <fubble/rtc/video_track.hpp>

namespace rtc {
namespace google {
class video_track : public rtc::google::track, public rtc::video_track {
public:
protected:
};
} // namespace google
} // namespace rtc

#endif
