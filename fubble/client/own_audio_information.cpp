#include "own_audio_information.hpp"
#include "audio_level_calculator.hpp"
#include "loopback_audio.hpp"
#include <fubble/rtc/audio_track.hpp>

using namespace client;

own_audio_information::own_audio_information(
    audio_level_calculator_factory &audio_level_calculator_factory_,
    loopback_audio &own_audio_)
    : audio_level_calculator_factory_(audio_level_calculator_factory_) {
  set_loopback_audio(own_audio_);
}

own_audio_information::~own_audio_information() = default;

void own_audio_information::set_loopback_audio(loopback_audio &audio) {
  signal_connections.clear();
  signal_connections.emplace_back(
      audio.on_track.connect([this](auto &track) { on_track(track); }));
  auto track = audio.get_track();
  if (!track)
    return;
  on_track(*track);
}

void own_audio_information::on_track(rtc::audio_track &track) {
  audio_level_calculator_ =
      audio_level_calculator_factory_.create(track.get_source());
  signal_connections.emplace_back(
      audio_level_calculator_->on_sound_level_30times_a_second.connect(
          [this](auto level) { on_sound_level_30times_a_second(level); }));
  signal_connections.emplace_back(
      audio_level_calculator_->on_voice_detected.connect(
          [this](auto detected) { on_voice_detected(detected); }));
}
