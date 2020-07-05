#include "rtc/google/track.hpp"

using namespace rtc::google;

track::track(
    rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> native_track_)
    : native_track_(native_track_) {}

rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
track::native_track() const {
  return native_track_;
}

void track::set_enabled(bool enabled) { native_track_->set_enabled(enabled); }

