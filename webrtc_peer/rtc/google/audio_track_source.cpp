#include "audio_track_source.hpp"

using namespace rtc::google;

audio_track_source::audio_track_source(
    rtc::scoped_refptr<webrtc::AudioTrackInterface> native_track_)
    : native_track_(native_track_) {}

rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
audio_track_source::native_track() const {
  return native_track_;
}
