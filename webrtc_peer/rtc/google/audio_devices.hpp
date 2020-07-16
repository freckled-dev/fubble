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

  // enumerate will not work unless the AudioDeviceModule got initialsed before
  // so do a track with yourself before enumerating
  void enumerate();
  struct device {
    int index{};
    std::string name;
  };
  using devices = std::vector<device>;
  devices get_playout_devices() const;
  devices get_recording_devices() const;

#if 0
  // there is no getter in googles webrtc
  int get_recording_device() const;
  int get_playout_device() const;
#endif
  void set_recording_device(int id);
  void set_output_device(int id);

  void mute_speaker(const bool mute);
  bool is_speaker_muted();
  void mute_microphone(const bool mute);
  bool is_microphone_muted();

  void start_recording();

  webrtc::AudioDeviceModule &get_native() const;

protected:
  void enumerate_on_thread();
  void set_recording_device_on_thread(int id);
  void set_output_device_on_thread(int id);

  rtc::logger logger{"audio_devices"};
  rtc::Thread &thread;
  webrtc::AudioDeviceModule &audio_device_module;
  devices recording_devices;
  devices playout_devices;
};
} // namespace google
} // namespace rtc

#endif
