#ifndef RTC_GOOGLE_TRACK_HPP
#define RTC_GOOGLE_TRACK_HPP

#include <fubble/rtc/google/google_webrtc_predecl.hpp>

namespace rtc {
namespace google {
class track {
public:
  virtual ~track() = default;

  virtual rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
  native_track() const = 0;

protected:
};
} // namespace google
} // namespace rtc

#endif
