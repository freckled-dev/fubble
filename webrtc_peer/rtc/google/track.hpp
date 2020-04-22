#ifndef RTC_GOOGLE_TRACK_HPP
#define RTC_GOOGLE_TRACK_HPP

#include "rtc/track.hpp"
#include <api/media_stream_interface.h>

namespace rtc {
namespace google {
class track : public rtc::track {
public:
  // TODO remove scoped_refptr?
  virtual rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
  native_track() const = 0;
};
} // namespace google
} // namespace rtc

#endif
