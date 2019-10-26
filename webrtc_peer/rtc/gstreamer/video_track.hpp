#ifndef RTC_GSTREAMER_VIDEO_TRACK_HPP
#define RTC_GSTREAMER_VIDEO_TRACK_HPP

#include "connection.hpp"
#include <logging/logger.hpp>
#include <rtc/track.hpp>
extern "C" {
#define GST_USE_UNSTABLE_API
#include <gst/webrtc/webrtc.h>
}

namespace rtc::gstreamer {

class video_track : public track {
public:
  struct parse_error : std::runtime_error {
    parse_error(GError *error);
  };

  video_track();
  ~video_track();
  void link_to_webrtc(const connection::natives &natives);

private:
  void initialise_gst();

  logging::logger logger;
  GstElement *bin;
  bool unref_bin{true};
};
} // namespace rtc::gstreamer

#endif

