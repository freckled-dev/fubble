#ifndef RTC_GSTREAMER_VIDEO_TRACK_HPP
#define RTC_GSTREAMER_VIDEO_TRACK_HPP

extern "C" {
#define GST_USE_UNSTABLE_API
#include <gst/webrtc/webrtc.h>
}

namespace rtc::gstreamer {
class video_track {
public:
  video_track();
};
} // namespace rtc::gstreamer

#endif

