#ifndef RTC_GSTREAMER_VIDEO_TRACK_HPP
#define RTC_GSTREAMER_VIDEO_TRACK_HPP

#include <fubble/rtc/gstreamer/connection.hpp>
#include <fubble/rtc/track.hpp>
#include <fubble/rtc/video_track.hpp>
#include <fubble/utils/logging/logger.hpp>
extern "C" {
#define GST_USE_UNSTABLE_API
#include <gst/webrtc/webrtc.h>
}

namespace rtc::gstreamer {

class video_track : public rtc::video_track {
public:
  struct parse_error : std::runtime_error {
    parse_error(GError *error);
  };

  video_track();
  ~video_track();
  void link_to_webrtc(const connection::natives &natives);

  void set_enabled(bool) override;

private:
  void initialise_gst();

  rtc::logger logger{"video_track"};
  GstElement *bin;
  bool unref_bin{true};
};
} // namespace rtc::gstreamer

#endif
