#include "audio_settings_module.hpp"
#include "fubble/client/audio_device_settings.hpp"
#include "fubble/client/audio_tracks_volume.hpp"
#include "fubble/client/loopback_audio.hpp"
#include "fubble/client/own_audio_information.hpp"
#include "fubble/client/own_microphone_tester.hpp"
#include "fubble/rtc/google/audio_track.hpp"
#include "fubble/rtc/google/capture/audio/device.hpp"
#include "fubble/rtc/google/factory.hpp"

using namespace client;

audio_settings_module::audio_settings_module(
    std::shared_ptr<utils::executor_module> executor_module,
    std::shared_ptr<rtc::google::module> rtc_module,
    std::shared_ptr<audio_module> audio_module_,
    std::shared_ptr<session_module> session_module_, const config &config_)
    : executor_module{executor_module}, rtc_module{rtc_module},
      audio_module_{audio_module_},
      session_module_{session_module_}, config_{config_} {}

std::shared_ptr<audio_tracks_volume>
audio_settings_module::get_audio_tracks_volume() {
  if (!audio_tracks_volume_)
    audio_tracks_volume_ = audio_tracks_volume::create(
        *session_module_->get_rooms(), *session_module_->get_tracks_adder(),
        audio_module_->get_own_audio_track_adder(),
        *audio_module_->get_own_audio_track());
  return audio_tracks_volume_;
}

std::shared_ptr<own_audio_information>
audio_settings_module::get_own_audio_test_information() {
  if (!own_audio_test_information_)
    own_audio_test_information_ = std::make_shared<own_audio_information>(
        *audio_module_->get_audio_level_calculator_factory(),
        *get_loopback_audio_test());
  return own_audio_test_information_;
}

std::shared_ptr<own_microphone_tester>
audio_settings_module::get_own_microphone_tester() {
  if (!own_microphone_tester_)
    own_microphone_tester_ = own_microphone_tester::create(
        *get_loopback_audio_test(), *get_audio_tracks_volume());
  return own_microphone_tester_;
}

std::shared_ptr<audio_device_settings>
audio_settings_module::get_audio_device_settings() {
  if (!audio_device_settings_) {
    auto &rtc_audio_devices = rtc_module->get_factory()->get_audio_devices();
    audio_device_settings_ =
        std::make_shared<audio_device_settings>(rtc_audio_devices);
  }
  return audio_device_settings_;
}

std::shared_ptr<loopback_audio>
audio_settings_module::get_loopback_audio_test() {
  if (!loopback_audio_test) {
    settings_audio_track = rtc_module->get_factory()->create_audio_track(
        rtc_module->get_audio_device()->get_source());
    loopback_audio_test_factory =
        std::make_shared<client::loopback_audio_impl_factory>(
            *rtc_module->get_factory(), settings_audio_track,
            executor_module->get_boost_executor());
    loopback_audio_test = std::make_shared<loopback_audio_noop_if_disabled>(
        *loopback_audio_test_factory);
  }
  return loopback_audio_test;
}
