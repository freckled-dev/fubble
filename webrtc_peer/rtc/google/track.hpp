#ifndef RTC_GOOGLE_TRACK_HPP
#define RTC_GOOGLE_TRACK_HPP

#include "rtc/track.hpp"
#include <api/media_stream_interface.h>

namespace rtc::google {
class track : public rtc::track {
public:
  virtual rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
  native_track() const = 0;
};
} // namespace rtc::google

#endif