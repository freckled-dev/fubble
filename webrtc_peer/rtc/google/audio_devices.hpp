#ifndef UUID_D7779D07_9C76_4C2F_A286_2EA21561E97E
#define UUID_D7779D07_9C76_4C2F_A286_2EA21561E97E

#include "rtc/logger.hpp"
#include <vector>

namespace rtc {
class Thread;
}
namespace webrtc {
class AudioDeviceModule;
} // namespace webrtc

namespace rtc {
namespace google {
class audio_devices {
public:
  audio_devices(rtc::Thread &thread,
                webrtc::AudioDeviceModule &audio_device_module);

  void enumerate();
  struct device {
    int index{};
    std::string name;
  };
  using devices = std::vector<device>;
  devices get_playout_devices() const;
  devices get_recording_devices() const;

  void mute_speaker(const bool mute);
  bool is_speaker_muted();
  void mute_microphone(const bool mute);
  bool is_microphone_muted();

  void start_recording();

  webrtc::AudioDeviceModule &get_native() const;

protected:
  void enumerate_on_thread();

  rtc::logger logger{"audio_devices"};
  rtc::Thread &thread;
  webrtc::AudioDeviceModule &audio_device_module;
  devices recording_devices;
  devices playout_devices;
};
} // namespace google
} // namespace rtc

#endif
