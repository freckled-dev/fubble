#include "own_media_model.hpp"
#include "client/own_audio_information.hpp"

using namespace client;

own_media_model::own_media_model(own_media &own_media_,
                                 own_audio_information &audio_information_)
    : own_media_(own_media_), audio_information_(audio_information_) {
  audio_information_.on_sound_level_30times_a_second.connect(
      [this](auto level) { on_sound_level(level); });
}

own_media_model::~own_media_model() = default;

void own_media_model::on_sound_level(const double level) {
  int audio_level = std::min(127, static_cast<int>(level * 127.0));
  newAudioLevel(audio_level);
}
