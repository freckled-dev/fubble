#include "audio_track_sink.hpp"
#include "audio_source.hpp"

using namespace rtc::google;

audio_track_sink::audio_track_sink(
    const rtc::scoped_refptr<webrtc::AudioTrackInterface> &track)
    : track(track) {
  source = std::make_unique<audio_source>(*track->GetSource());
}

audio_track_sink::~audio_track_sink() = default;

rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
audio_track_sink::native_track() const {
  return track;
}

webrtc::AudioTrackInterface &audio_track_sink::get_native_audio_track() {
  return *track;
}

audio_source &audio_track_sink::get_source() { return *source; }
