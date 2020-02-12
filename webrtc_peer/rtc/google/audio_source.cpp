#include "audio_source.hpp"

using namespace rtc::google;

audio_source::audio_source(webrtc::AudioSourceInterface &native)
    : native_audio_track(native) {}

audio_source::~audio_source() {}

webrtc::AudioSourceInterface &audio_source::get_native() const {
  return native_audio_track;
}

#if 0
void audio_source::OnData(const void *audio_data, int bits_per_sample,
                          int sample_rate, size_t number_of_channels,
                          size_t number_of_frames) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "OnData, number_of_frames:" << number_of_frames << ", this:" << this;
  struct audio_data result;
  result.audio_data = audio_data;
  result.bits_per_sample = bits_per_sample;
  result.sample_rate = sample_rate;
  result.number_of_channels = number_of_channels;
  result.number_of_frames = number_of_frames;
  on_data(result);
}
#endif
