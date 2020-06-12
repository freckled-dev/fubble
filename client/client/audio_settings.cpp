#include "audio_settings.hpp"
#include "rtc/google/audio_devices.hpp"

using namespace client;

audio_settings::audio_settings(rtc::google::audio_devices &rtc_audio_devices)
    : rtc_audio_devices(rtc_audio_devices) {}

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

