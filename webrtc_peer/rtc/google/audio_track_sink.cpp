#include "audio_track_sink.hpp"

using namespace rtc::google;

audio_track_sink::audio_track_sink(
    const rtc::scoped_refptr<webrtc::AudioTrackInterface> &track)
    : audio_source(*track->GetSource()), track(track) {}

rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
audio_track_sink::native_track() const {
  return track;
}
