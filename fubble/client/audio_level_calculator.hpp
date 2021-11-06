#ifndef UUID_63CC2CDA_533D_43C1_A9E7_51D617CDC08C
#define UUID_63CC2CDA_533D_43C1_A9E7_51D617CDC08C

#include <fubble/utils/signal.hpp>
#include <boost/thread/executors/executor.hpp>
#include <fubble/client/logger.hpp>
#include <fubble/rtc/audio_data.hpp>
#include <fubble/rtc/audio_source.hpp>

namespace rtc::google {
class voice_detection;
} // namespace rtc::google

namespace webrtc {
class VoiceDetection;
}
namespace client {
class audio_level_calculator {
public:
  audio_level_calculator(boost::executor &main_thread,
                         rtc::audio_source &audio_source_);
  ~audio_level_calculator();

  const rtc::audio_source &get_source() const;

  // TODO refactor to 10
  utils::signal::signal<double> on_sound_level_30times_a_second;
  utils::signal::signal<bool> on_voice_detected;

protected:
  void on_data(const rtc::audio_data &data);
  void calculate_30times_a_second(double new_level);
  void calculate_voice_detection(const rtc::audio_data &data);

  client::logger logger{"audio_level_calculator"};
  boost::executor &main_thread;
  rtc::audio_source &audio_source;
  std::unique_ptr<rtc::google::voice_detection> voice_detection_;

  static constexpr int audio_level_values_to_collect{100 / 10};
  double audio_level_cache{};
  int audio_level_counter{};

  bool voice_detected{};
  std::shared_ptr<int> alive_check = std::make_shared<int>(42);
  utils::signal::scoped_connection on_data_connection;
};

class audio_level_calculator_factory {
public:
  audio_level_calculator_factory(boost::executor &main_thread)
      : main_thread(main_thread) {}

  std::unique_ptr<audio_level_calculator>
  create(rtc::audio_source &audio_source_) {
    return std::make_unique<audio_level_calculator>(main_thread, audio_source_);
  }

protected:
  boost::executor &main_thread;
};

} // namespace client

#endif
