#include "own_audio_information.hpp"
#include "audio_level_calculator.hpp"
#include "own_audio.hpp"
#include "rtc/google/audio_track.hpp"

using namespace client;

own_audio_information::own_audio_information(own_audio &own_audio_) {
  own_audio_.on_track.connect([this](auto &track) { on_track(track); });
}

own_audio_information::~own_audio_information() = default;

void own_audio_information::on_track(rtc::google::audio_track &track) {
  audio_level_calculator_ =
      std::make_unique<audio_level_calculator>(track.get_source());
}
