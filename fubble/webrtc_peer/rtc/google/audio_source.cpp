#include "audio_source.hpp"
#include <api/media_stream_interface.h>

using namespace rtc::google;

struct audio_source::sink : webrtc::AudioTrackSinkInterface {

  sink(webrtc::AudioSourceInterface &source) : source(source) {
#if 1
    BOOST_LOG_SEV(logger, logging::severity::trace)
        << __FUNCTION__ << ", this:" << this;
#endif
    source.AddSink(this);
  }

  ~sink() {
#if 1
    BOOST_LOG_SEV(logger, logging::severity::trace)
        << __FUNCTION__ << ", this:" << this;
#endif

    source.RemoveSink(this);
  }

  void OnData(const void *audio_data, int bits_per_sample, int sample_rate,
              std::size_t number_of_channels, std::size_t number_of_frames,
              absl::optional<int64_t> absolute_capture_timestamp_ms) override {
    (void)absolute_capture_timestamp_ms;
#if 1
    BOOST_LOG_SEV(logger, logging::severity::trace)
        << "OnData, number_of_frames:" << number_of_frames << ", this:" << this;
#endif
    struct audio_data result;
    result.audio_data = audio_data;
    result.bits_per_sample = bits_per_sample;
    result.sample_rate = sample_rate;
    result.number_of_channels = number_of_channels;
    result.number_of_frames = number_of_frames;
    on_data(result);
  }
  boost::signals2::signal<void(const audio_data &)> on_data;

  rtc::logger logger{"audio_source::sink"};
  webrtc::AudioSourceInterface &source;
};

audio_source::audio_source(webrtc::AudioSourceInterface &native)
    : native_audio_track(native) {
  sink_ = std::make_unique<sink>(native);
  sink_->on_data.connect([this](auto data) { on_data(data); });
}

audio_source::~audio_source() = default;

webrtc::AudioSourceInterface &audio_source::get_native() const {
  return native_audio_track;
}
