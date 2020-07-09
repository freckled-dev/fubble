#include "audio_settings.hpp"
#include "rtc/google/audio_devices.hpp"

using namespace client;

audio_settings::audio_settings(rtc::google::audio_devices &rtc_audio_devices)
    : rtc_audio_devices(rtc_audio_devices) {}

void audio_settings::set_recording_device(int id) {
  rtc_audio_devices.set_recording_device(id);
}

int audio_settings::get_recording_device() const {
  return rtc_audio_devices.get_recording_device();
}

void audio_settings::set_output_device(int id) {
  rtc_audio_devices.set_output_device(id);
}

int audio_settings::get_playout_device() const {
  return rtc_audio_devices.get_playout_device();
}

void audio_settings::mute_speaker(const bool mute) {
  rtc_audio_devices.mute_speaker(mute);
}

bool audio_settings::is_speaker_muted() {
  return rtc_audio_devices.is_speaker_muted();
}

void audio_settings::mute_microphone(const bool mute) {
  return rtc_audio_devices.mute_microphone(mute);
}

bool audio_settings::is_microphone_muted() {
  return rtc_audio_devices.is_microphone_muted();
}
