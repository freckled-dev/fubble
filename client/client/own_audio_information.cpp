#include "own_audio_information.hpp"
#include "audio_level_calculator.hpp"
#include "loopback_audio.hpp"
#include "rtc/google/audio_track.hpp"

using namespace client;

own_audio_information::own_audio_information(loopback_audio &own_audio_) {
  own_audio_.on_track.connect([this](auto &track) { on_track(track); });
  // on_track(own_audio_.get_track());
}

own_audio_information::~own_audio_information() = default;

void own_audio_information::on_track(rtc::google::audio_track &track) {
  audio_level_calculator_ =
      std::make_unique<audio_level_calculator>(track.get_source());
  audio_level_calculator_->on_sound_level_30times_a_second.connect(
      [this](auto level) { on_sound_level_30times_a_second(level); });
  audio_level_calculator_->on_voice_detected.connect(
      [this](auto detected) { on_voice_detected(detected); });
}
