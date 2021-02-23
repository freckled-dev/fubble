#ifndef RTC_GOOGLE_VIDEO_TRACK_HPP
#define RTC_GOOGLE_VIDEO_TRACK_HPP

#include <fubble/rtc/google/track.hpp>
#include <fubble/rtc/video_track.hpp>
#include <media/base/adapted_video_track_source.h>

namespace rtc {
namespace google {
class video_track : public track, public rtc::video_track {
public:
  video_track(const rtc::scoped_refptr<webrtc::VideoTrackInterface> track);
  ~video_track();

  void set_enabled(bool enabled) override;

protected:
};
} // namespace google
} // namespace rtc

#endif
