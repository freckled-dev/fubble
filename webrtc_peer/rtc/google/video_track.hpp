#ifndef RTC_GOOGLE_VIDEO_TRACK_HPP
#define RTC_GOOGLE_VIDEO_TRACK_HPP

#include "track.hpp"
#include <media/base/adapted_video_track_source.h>

namespace rtc::google {
class video_track : public track {
public:
  video_track(const rtc::scoped_refptr<webrtc::VideoTrackInterface> &track);
  ~video_track();

protected:
  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
  native_track() const override;

  const rtc::scoped_refptr<webrtc::VideoTrackInterface> track;
};
} // namespace rtc::google

#endif
