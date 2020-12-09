#ifndef UUID_2761C87F_C48F_4545_BAED_5C5E847EDF9C
#define UUID_2761C87F_C48F_4545_BAED_5C5E847EDF9C

#include "rtc/google/module.hpp"
#include "utils/export.hpp"

namespace client {
class audio_level_calculator_factory;
class own_audio_track;
class add_audio_to_connection;
class loopback_audio;
class own_audio_information;
class FUBBLE_PUBLIC audio_module {
public:
  struct config {};

  audio_module(std::shared_ptr<utils::executor_module> executor_module,
               std::shared_ptr<rtc::google::module> rtc_module,
               const config &config_);

  std::shared_ptr<own_audio_track> get_own_audio_track();
  // TODO rename to get_own_audio_track_adder();
  std::shared_ptr<add_audio_to_connection> get_own_audio_track_adder();
  std::shared_ptr<loopback_audio> get_loopback_audio();
  std::shared_ptr<audio_level_calculator_factory>
  get_audio_level_calculator_factory();
  std::shared_ptr<own_audio_information> get_own_audio_information();

protected:
  std::shared_ptr<utils::executor_module> executor_module;
  std::shared_ptr<rtc::google::module> rtc_module;
  const config config_;

  std::shared_ptr<own_audio_track> own_audio_track_;
  std::shared_ptr<add_audio_to_connection> audio_track_adder;
  std::shared_ptr<loopback_audio> loopback_audio_;
  std::shared_ptr<audio_level_calculator_factory>
      audio_level_calculator_factory_;
  std::shared_ptr<own_audio_information> own_audio_information_;
};
} // namespace client

#endif
