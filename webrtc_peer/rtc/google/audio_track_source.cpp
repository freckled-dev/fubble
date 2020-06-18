#include "audio_track_source.hpp"
#include "audio_source.hpp"

using namespace rtc::google;

audio_track_source::audio_track_source(
    rtc::scoped_refptr<webrtc::AudioTrackInterface> native_track_,
    audio_source &source)
    : native_track_(native_track_), source(source) {}

rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
audio_track_source::native_track() const {
  return native_track_;
}

webrtc::AudioTrackInterface &audio_track_source::get_native_audio_track() {
  return *native_track_;
}

audio_source &audio_track_source::get_source() { return source; }
