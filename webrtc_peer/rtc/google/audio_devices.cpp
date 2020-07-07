#include "audio_devices.hpp"
#include <audio_device/include/audio_device.h>
#include <audio_device/include/audio_device_data_observer.h>
#include <rtc_base/thread.h>

using namespace rtc::google;

audio_devices::audio_devices(rtc::Thread &thread,
                             webrtc::AudioDeviceModule &audio_device_module)
    : thread(thread), audio_device_module(audio_device_module) {}

void audio_devices::enumerate() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  thread.Invoke<void>(RTC_FROM_HERE, [this]() { enumerate_on_thread(); });
}

audio_devices::devices audio_devices::get_playout_devices() const {
  return playout_devices;
}

audio_devices::devices audio_devices::get_recording_devices() const {
  return recording_devices;
}

void audio_devices::mute_speaker(const bool mute) {
  thread.Invoke<void>(RTC_FROM_HERE, [this, mute]() {
#ifndef NDEBUG
    bool ok{};
    audio_device_module.SpeakerMuteIsAvailable(&ok);
    BOOST_ASSERT(ok);
#endif
    auto result = audio_device_module.SetSpeakerMute(mute);
    BOOST_ASSERT(result == 0);
    if (result == 0)
      return;
    BOOST_LOG_SEV(this->logger, logging::severity::warning)
        << "could not set speaker mute to:" << mute;
  });
}

bool audio_devices::is_speaker_muted() {
  return thread.Invoke<bool>(RTC_FROM_HERE, [this]() {
    bool muted{};
    [[maybe_unused]] auto result = audio_device_module.SpeakerMute(&muted);
    BOOST_ASSERT(result == 0);
    if (result != 0)
      BOOST_LOG_SEV(this->logger, logging::severity::warning)
          << "could not get speaker mute";
    return muted;
  });
}

void audio_devices::mute_microphone(const bool mute) {
  thread.Invoke<void>(RTC_FROM_HERE, [this, mute]() {
#ifndef NDEBUG
    bool ok{};
    audio_device_module.MicrophoneMuteIsAvailable(&ok);
    BOOST_ASSERT(ok);
#endif
    auto result = audio_device_module.SetMicrophoneMute(mute);
    BOOST_ASSERT(result == 0);
    if (result == 0)
      return;
    BOOST_LOG_SEV(this->logger, logging::severity::warning)
        << "could not set microphone mute to:" << mute;
  });
}

bool audio_devices::is_microphone_muted() {
  return thread.Invoke<bool>(RTC_FROM_HERE, [this]() {
    bool muted{};
    auto result = audio_device_module.MicrophoneMute(&muted);
    BOOST_ASSERT(result == 0);
    if (result != 0)
      BOOST_LOG_SEV(this->logger, logging::severity::warning)
          << "could not get microphone mute";
    return muted;
  });
}

void audio_devices::start_recording() {
  thread.Invoke<void>(RTC_FROM_HERE, [this]() {
    auto result = audio_device_module.StartRecording();
    BOOST_ASSERT(result == 0);
    if (result != 0)
      BOOST_LOG_SEV(this->logger, logging::severity::warning)
          << "could not get microphone mute";
  });
}

webrtc::AudioDeviceModule &audio_devices::get_native() const {
  return audio_device_module;
}

void audio_devices::enumerate_on_thread() {
  auto playout_count = audio_device_module.PlayoutDevices();
  auto recording_count = audio_device_module.RecordingDevices();

  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "playout_count:" << playout_count
      << ", recording_count:" << recording_count;

  for (int index{}; index < playout_count; ++index) {
    std::array<char, webrtc::kAdmMaxDeviceNameSize> name;
    std::array<char, webrtc::kAdmMaxGuidSize> guid;
    auto result =
        audio_device_module.PlayoutDeviceName(index, name.data(), guid.data());
    BOOST_ASSERT(result == 0);
    if (result != 0) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << "could not get playout device name";
      continue;
    }
    const std::string name_casted = name.data();
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "playout_device name:" << name_casted;
    playout_devices.push_back({index, name_casted});
  }
  for (int index{}; index < recording_count; ++index) {
    std::array<char, webrtc::kAdmMaxDeviceNameSize> name;
    std::array<char, webrtc::kAdmMaxGuidSize> guid;
    auto result = audio_device_module.RecordingDeviceName(index, name.data(),
                                                          guid.data());
    BOOST_ASSERT(result == 0);
    if (result != 0) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << "could not get recording device name";
      continue;
    }
    const std::string name_casted = name.data();
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "recording_device name:" << name_casted;
    recording_devices.push_back({index, name_casted});
  }
}
