#include "audio_device_settings.hpp"
#include "fubble/rtc/google/audio_devices.hpp"

using namespace client;

audio_device_settings::audio_device_settings(
    rtc::google::audio_devices &rtc_audio_devices)
    : rtc_audio_devices(rtc_audio_devices) {}

void audio_device_settings::set_recording_device(int id) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", id:" << id;
  current_recording_device = id;
  rtc_audio_devices.set_recording_device(id);
}

int audio_device_settings::get_recording_device() const {
  return current_recording_device;
}

void audio_device_settings::set_output_device(int id) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", id:" << id;
  current_output_device = id;
  rtc_audio_devices.set_output_device(id);
}

int audio_device_settings::get_playout_device() const {
  return current_output_device;
}

void audio_device_settings::mute_speaker(const bool mute) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", mute:" << mute;
  rtc_audio_devices.mute_speaker(mute);
}

bool audio_device_settings::is_speaker_muted() {
  return rtc_audio_devices.is_speaker_muted();
}

void audio_device_settings::mute_microphone(const bool mute) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", mute:" << mute;
  return rtc_audio_devices.mute_microphone(mute);
}

bool audio_device_settings::is_microphone_muted() {
  return rtc_audio_devices.is_microphone_muted();
}
