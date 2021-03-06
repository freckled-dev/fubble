#ifndef UUID_3AF1EB16_2810_4C69_9A46_59A98C68EAB5
#define UUID_3AF1EB16_2810_4C69_9A46_59A98C68EAB5

#include "fubble/client/logger.hpp"
#include <boost/signals2/signal.hpp>

namespace rtc::google {
class audio_track;
}

namespace client {
class loopback_audio;
class audio_level_calculator;
class audio_level_calculator_factory;
class own_audio_information {
public:
  own_audio_information(
      audio_level_calculator_factory &audio_level_calculator_factory_,
      loopback_audio &own_audio_);
  ~own_audio_information();

  boost::signals2::signal<void(double)> on_sound_level_30times_a_second;
  boost::signals2::signal<void(bool)> on_voice_detected;

protected:
  void set_loopback_audio(loopback_audio &audio);
  void on_track(rtc::google::audio_track &);

  audio_level_calculator_factory &audio_level_calculator_factory_;
  std::unique_ptr<audio_level_calculator> audio_level_calculator_;
  std::vector<boost::signals2::scoped_connection> signal_connections;
};
} // namespace client

#endif
