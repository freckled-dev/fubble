#include "audio_track_sink.hpp"
#include "audio_source.hpp"

using namespace rtc::google;

audio_track_sink::audio_track_sink(
    const rtc::scoped_refptr<webrtc::AudioTrackInterface> &track_)
    : native_track_(track_) {
  source = std::make_unique<audio_source>(*track_->GetSource());
}

audio_track_sink::~audio_track_sink() = default;

webrtc::AudioTrackInterface &audio_track_sink::get_native_audio_track() {
  return *native_track_;
}

rtc::audio_source &audio_track_sink::get_source() { return *source; }

void audio_track_sink::set_volume(double volume) {
  BOOST_ASSERT(native_track_->GetSource());
  native_track_->GetSource()->SetVolume(volume);
}

rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
audio_track_sink::native_track() const {
  return native_track_;
}

void audio_track_sink::set_enabled(bool enabled) {
  native_track_->set_enabled(enabled);
}
