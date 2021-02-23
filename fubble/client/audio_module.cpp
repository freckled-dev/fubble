#include "audio_module.hpp"
#include <fubble/client/add_audio_to_connection.hpp>
#include <fubble/client/audio_level_calculator.hpp>
#include <fubble/client/loopback_audio.hpp>
#include <fubble/client/own_audio_information.hpp>
#include <fubble/client/own_audio_track.hpp>
#include <fubble/rtc/google/capture/audio/device.hpp>
#include <fubble/rtc/google/factory.hpp>
#include <fubble/utils/executor_module.hpp>

using namespace client;

audio_module::audio_module(
    std::shared_ptr<utils::executor_module> executor_module,
    std::shared_ptr<rtc::google::module> rtc_module, const config &config_)
    : executor_module{executor_module}, rtc_module{rtc_module}, config_{
                                                                    config_} {}

std::shared_ptr<own_audio_track> audio_module::get_own_audio_track() {
  if (!own_audio_track_)
    own_audio_track_ =
        own_audio_track::create(*std::static_pointer_cast<rtc::google::factory>(
                                    rtc_module->get_factory()),
                                rtc_module->get_audio_device()->get_source());
  return own_audio_track_;
}

std::shared_ptr<add_audio_to_connection>
audio_module::get_own_audio_track_adder() {
  if (!audio_track_adder)
    audio_track_adder =
        add_audio_to_connection::create(get_own_audio_track()->get_track());
  return audio_track_adder;
}

std::shared_ptr<loopback_audio> audio_module::get_loopback_audio() {
  if (!loopback_audio_)
    loopback_audio_ =
        loopback_audio::create(*std::static_pointer_cast<rtc::google::factory>(
                                   rtc_module->get_factory()),
                               get_own_audio_track()->get_track(),
                               executor_module->get_boost_executor());
  return loopback_audio_;
}

std::shared_ptr<audio_level_calculator_factory>
audio_module::get_audio_level_calculator_factory() {
  if (!audio_level_calculator_factory_)
    audio_level_calculator_factory_ =
        std::make_shared<audio_level_calculator_factory>(
            *executor_module->get_boost_executor());
  return audio_level_calculator_factory_;
}

std::shared_ptr<own_audio_information>
audio_module::get_own_audio_information() {
  if (!own_audio_information_)
    own_audio_information_ = std::make_shared<own_audio_information>(
        *get_audio_level_calculator_factory(), *get_loopback_audio());
  return own_audio_information_;
}
