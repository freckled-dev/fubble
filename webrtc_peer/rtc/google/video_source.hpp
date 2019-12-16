#ifndef RTC_GOOGLE_VIDEO_SOURCE_HPP
#define RTC_GOOGLE_VIDEO_SOURCE_HPP

#include <api/video/video_frame.h>
#include <boost/signals2/signal.hpp>

namespace rtc::google {
class video_source {
public:
  virtual ~video_source();
  boost::signals2::signal<void(const webrtc::VideoFrame &)> on_frame;
};
} // namespace rtc::google

#endif

