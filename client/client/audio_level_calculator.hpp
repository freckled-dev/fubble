#ifndef UUID_63CC2CDA_533D_43C1_A9E7_51D617CDC08C
#define UUID_63CC2CDA_533D_43C1_A9E7_51D617CDC08C

#include "client/logger.hpp"
#include <boost/signals2/signal.hpp>

namespace rtc::google {
class audio_source;
struct audio_data;
} // namespace rtc::google

namespace client {
class audio_level_calculator {
public:
  audio_level_calculator(rtc::google::audio_source &audio_source_);

  const rtc::google::audio_source &get_source() const;

  // gets called 100 times a second
  boost::signals2::signal<void(double)> on_sound_level;
  // TODO refactor to 10
  boost::signals2::signal<void(double)> on_sound_level_30times_a_second;
  boost::signals2::signal<void(bool)> on_voice_detected;

protected:
  void on_data(rtc::google::audio_data &data);
  void calculate_30times_a_second(double new_level);
  void calculate_voice_detection(double new_level);

  client::logger logger{"audio_level_calculator"};
  rtc::google::audio_source &audio_source;
  boost::signals2::scoped_connection on_data_connection;

  static constexpr int audio_level_values_to_collect{100 / 10};
  double audio_level_cache{};
  int audio_level_counter{};

  static constexpr int voice_audio_level_values_to_collect{100 / 30};
  static constexpr double voice_detected_threshold{0.1};
  bool voice_detected{};
  double voice_detected_audio_level_cache{};
  int voice_detected_counter{};
};
} // namespace client

#endif
