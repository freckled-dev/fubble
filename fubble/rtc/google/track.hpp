#ifndef RTC_GOOGLE_TRACK_HPP
#define RTC_GOOGLE_TRACK_HPP

#include "fubble/rtc/track.hpp"
#include <api/media_stream_interface.h>

namespace rtc {
namespace google {
class track {
public:
  track(rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> native_track);

  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> native_track() const;

  void set_enabled(bool);

protected:
  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> native_track_;
};
} // namespace google
} // namespace rtc

#endif
