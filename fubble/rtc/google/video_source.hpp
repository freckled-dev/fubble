#ifndef RTC_GOOGLE_VIDEO_SOURCE_HPP
#define RTC_GOOGLE_VIDEO_SOURCE_HPP

#include <api/video/video_frame.h>
#include <boost/signals2/signal.hpp>
#include <fubble/rtc/video_source.hpp>

namespace rtc {
namespace google {
// TODO do a `rtc::video_source`. before that, do a container for
// `webrtc::VideoFrame`
class video_source : public rtc::video_source {
public:
  virtual ~video_source();
  boost::signals2::signal<void(const webrtc::VideoFrame &)> on_frame;
};
} // namespace google
} // namespace rtc

#endif
