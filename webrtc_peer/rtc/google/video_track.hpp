#ifndef RTC_GOOGLE_VIDEO_TRACK_HPP
#define RTC_GOOGLE_VIDEO_TRACK_HPP

#include "track.hpp"
#include <media/base/adapted_video_track_source.h>

namespace rtc {
namespace google {
class video_track : public track {
public:
  video_track(const rtc::scoped_refptr<webrtc::VideoTrackInterface> track);
  ~video_track();

protected:
};
} // namespace google
} // namespace rtc

#endif
