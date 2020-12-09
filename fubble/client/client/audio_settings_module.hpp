#ifndef UUID_DA215727_927E_49D3_A600_CBD9C8E62A7F
#define UUID_DA215727_927E_49D3_A600_CBD9C8E62A7F

#include "client/audio_module.hpp"
#include "client/session_module.hpp"
#include "rtc/google/module.hpp"
#include "utils/export.hpp"

namespace rtc::google {
class audio_track;
}
namespace client {
class loopback_audio_factory;
class audio_tracks_volume;
class own_microphone_tester;
class audio_device_settings;
class FUBBLE_PUBLIC audio_settings_module {
public:
  struct config {};
  audio_settings_module(std::shared_ptr<utils::executor_module> executor_module,
                        std::shared_ptr<rtc::google::module> rtc_module,
                        std::shared_ptr<audio_module> audio_module_,
                        std::shared_ptr<session_module> session_module_,
                        const config &config_);

  std::shared_ptr<audio_tracks_volume> get_audio_tracks_volume();
  std::shared_ptr<own_audio_information> get_own_audio_test_information();
  std::shared_ptr<own_microphone_tester> get_own_microphone_tester();
  std::shared_ptr<audio_device_settings> get_audio_device_settings();

protected:
  std::shared_ptr<loopback_audio> get_loopback_audio_test();

  std::shared_ptr<utils::executor_module> executor_module;
  std::shared_ptr<rtc::google::module> rtc_module;
  std::shared_ptr<audio_module> audio_module_;
  std::shared_ptr<session_module> session_module_;
  const config &config_;

  // TODO adds audio_track_adder to tracks_adder - refactor!
  std::shared_ptr<audio_tracks_volume> audio_tracks_volume_;
  std::shared_ptr<rtc::google::audio_track> settings_audio_track;
  std::shared_ptr<loopback_audio_factory> loopback_audio_test_factory;
  std::shared_ptr<loopback_audio> loopback_audio_test;
  std::shared_ptr<own_microphone_tester> own_microphone_tester_;
  std::shared_ptr<own_audio_information> own_audio_test_information_;
  std::shared_ptr<audio_device_settings> audio_device_settings_;
};
} // namespace client

#endif
