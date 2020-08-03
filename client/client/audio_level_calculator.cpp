#include "audio_level_calculator.hpp"
#include "rtc/google/audio_source.hpp"
#include "rtc/google/voice_detection.hpp"
#include <numeric>

using namespace client;

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

audio_level_calculator::audio_level_calculator(
    rtc::google::audio_source &audio_source)
    : audio_source(audio_source) {
  on_data_connection =
      audio_source.on_data.connect([this](auto data) { on_data(data); });
  voice_detection_ = rtc::google::voice_detection::create();
}

audio_level_calculator::~audio_level_calculator() = default;

const rtc::google::audio_source &audio_level_calculator::get_source() const {
  return audio_source;
}

void audio_level_calculator::on_data(const rtc::google::audio_data &data) {
  auto bits_per_sample = data.bits_per_sample;
  auto audio_data = data.audio_data;
  auto number_of_frames = data.number_of_frames;
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
  calculate_30times_a_second(averaged);
  calculate_voice_detection(data);
}

void audio_level_calculator::calculate_30times_a_second(double new_level) {
  ++audio_level_counter;
  if (audio_level_counter > audio_level_values_to_collect) {
    audio_level_cache = std::min(1.0, audio_level_cache);
#if 0
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "on_sound_level_30times_a_second, level:" << audio_level_cache;
#endif
    on_sound_level_30times_a_second(audio_level_cache);
    audio_level_counter = 0;
    audio_level_cache = 0.;
  }
  audio_level_cache +=
      new_level / static_cast<double>(audio_level_values_to_collect);
}

void audio_level_calculator::calculate_voice_detection(
    const rtc::google::audio_data &data) {
  bool check = voice_detection_->detect(data);
  if (check == voice_detected)
    return;
  voice_detected = check;
  on_voice_detected(voice_detected);
}
