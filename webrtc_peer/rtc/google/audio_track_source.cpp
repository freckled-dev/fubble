#include "audio_track_source.hpp"
#include "audio_source.hpp"

using namespace rtc::google;

namespace {
struct sink : webrtc::AudioTrackSinkInterface {

  sink(webrtc::AudioTrackInterface &source) {
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
  }

  void OnData(const void *audio_data, int bits_per_sample, int sample_rate,
              std::size_t number_of_channels, std::size_t number_of_frames,
              absl::optional<int64_t> absolute_capture_timestamp_ms) override {
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

  rtc::logger logger{"audio_track_source::sink"};
};
} // namespace

audio_track_source::audio_track_source(
    rtc::scoped_refptr<webrtc::AudioTrackInterface> native_track_,
    audio_source &source)
    : native_track_(native_track_), source(source) {
  new sink(*native_track_);
}

rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
audio_track_source::native_track() const {
  return native_track_;
}

webrtc::AudioTrackInterface &audio_track_source::get_native_audio_track() {
  return *native_track_;
}

audio_source &audio_track_source::get_source() { return source; }
