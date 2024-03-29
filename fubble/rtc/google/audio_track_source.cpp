#include "audio_track_source.hpp"
#include "audio_source.hpp"
#include <api/media_stream_interface.h>

using namespace rtc::google;

audio_track_source::audio_track_source(
    rtc::scoped_refptr<webrtc::AudioTrackInterface> native_track_,
    rtc::audio_source &source)
    : native_track_(native_track_), source(source) {}

audio_track_source::~audio_track_source() = default;

webrtc::AudioTrackInterface &audio_track_source::get_native_audio_track() {
  return *native_track_;
}

rtc::audio_source &audio_track_source::get_source() { return source; }

void audio_track_source::set_volume(double volume) {
  BOOST_ASSERT(native_track_->GetSource());
  native_track_->GetSource()->SetVolume(volume);
}

void audio_track_source::set_enabled(bool enabled) {
  native_track_->set_enabled(enabled);
}

rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
audio_track_source::native_track() const {
  return native_track_;
}
