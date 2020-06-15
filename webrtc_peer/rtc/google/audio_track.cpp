#include "audio_track.hpp"

using namespace rtc::google;

namespace {
struct bla : webrtc::AudioTrackSinkInterface {
  void OnData(const void *audio_data, int bits_per_sample, int sample_rate,
              size_t number_of_channels, size_t number_of_frames,
              absl::optional<int64_t> absolute_capture_timestamp_ms) override {
    //
  }
};
} // namespace

audio_track::stats audio_track::get_stats() {
  stats result;
  webrtc::AudioTrackInterface &native = get_native_audio_track();
  webrtc::AudioSourceInterface *source = native.GetSource();
  BOOST_ASSERT(source);
  auto options = source->options();
  source->AddSink(new bla);
  {
    int level{};
    bool ok = native.GetSignalLevel(&level);
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "level:" << level << ", ok:" << ok;
  }
  auto audio_processor = native.GetAudioProcessor();
  // BOOST_ASSERT(audio_processor);
  if (!audio_processor) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "audio_processor is nullptr";
    return result;
  }
  bool has_remote_tracks{true};
  webrtc::AudioProcessingStats stats_ =
      audio_processor->GetStats(has_remote_tracks).apm_statistics;
  if (stats_.output_rms_dbfs)
    result.rms_dbfs = stats_.output_rms_dbfs.value();
  if (stats_.voice_detected)
    result.voice_detected = stats_.voice_detected.value();
  return result;
}
