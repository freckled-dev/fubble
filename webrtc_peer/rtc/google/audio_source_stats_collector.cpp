#include "audio_source_stats_collector.hpp"
#include "audio_source.hpp"
#include <api/media_stream_interface.h>
#include <limits>
#include <numeric>

using namespace rtc::google;

namespace {
template <typename number_type>
double average(const number_type *data, std::size_t count) {
  auto sum = std::accumulate(
      data, data + count, std::int64_t{},
      [](auto result, auto input) { return result += std::abs(input); });
  auto averaged = sum /= count;
  long double result =
      static_cast<long double>(averaged) /
      static_cast<long double>(std::numeric_limits<number_type>::max());
  return static_cast<double>(result) * 2.;
}
} // namespace

struct audio_source_stats_collector::collector
    : webrtc::AudioTrackSinkInterface {
  void OnData(const void *audio_data, int bits_per_sample, int sample_rate,
              std::size_t number_of_channels, std::size_t number_of_frames,
              absl::optional<int64_t> absolute_capture_timestamp_ms) override {
    double averaged = [&] {
      auto frame_count = number_of_frames; // * number_of_channels?
      if (bits_per_sample == 8) {
        auto data_casted = static_cast<const std::int8_t *>(audio_data);
        return average(data_casted, frame_count);
      }
      if (bits_per_sample == 16) {
        auto data_casted = static_cast<const std::int16_t *>(audio_data);
        return average(data_casted, frame_count);
      }
      if (bits_per_sample == 32) {
        auto data_casted = static_cast<const std::int64_t *>(audio_data);
        return average(data_casted, frame_count);
      }
      BOOST_ASSERT(false);
      return 0.;
    }();
#if 0
    BOOST_LOG_SEV(logger, logging::severity::trace)
        << "OnData, bits_per_sample:" << bits_per_sample
        << ", number_of_frames:" << number_of_frames
        << ", sample_rate:" << sample_rate << ", averaged:" << averaged;
#endif
    on_sound_level(averaged);
  }

  boost::signals2::signal<void(double)> on_sound_level;
  rtc::logger logger{"audio_source_stats_collector::collector"};
};

audio_source_stats_collector::audio_source_stats_collector(audio_source &source)
    : source(source) {}

audio_source_stats_collector::~audio_source_stats_collector() {
  if (!collector_)
    return;
  stop();
}

void audio_source_stats_collector::start() {
  webrtc::AudioSourceInterface &native = source.get_native();
  BOOST_ASSERT(!collector_);
  if (collector_)
    return;
  collector_ = std::make_unique<collector>();
  collector_->on_sound_level.connect(
      [this](auto rate) { on_sound_level(rate); });
  native.AddSink(collector_.get());
}

void audio_source_stats_collector::stop() {
  webrtc::AudioSourceInterface &native = source.get_native();
  BOOST_ASSERT(collector_);
  if (!collector_)
    return;
  native.RemoveSink(collector_.get());
  collector_.reset();
}
