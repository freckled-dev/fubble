#ifndef UUID_3F220ACF_9A73_441D_911A_A2585AD9619D
#define UUID_3F220ACF_9A73_441D_911A_A2585AD9619D

namespace rtc::google {
class audio_devices;
}

namespace client {
class audio_settings {
public:
  audio_settings(rtc::google::audio_devices &rtc_audio_devices);

  void set_recording_device(int id);
  int get_recording_device() const;
  void set_output_device(int id);
  int get_playout_device() const;

  void mute_speaker(const bool mute);
  bool is_speaker_muted();
  void mute_microphone(const bool mute);
  bool is_microphone_muted();

protected:
  rtc::google::audio_devices &rtc_audio_devices;
  int current_output_device{};
  int current_recording_device{};
};
} // namespace client

#endif
