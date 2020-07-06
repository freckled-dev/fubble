#include "own_media.hpp"
#include "own_audio.hpp"
#include "rtc/google/audio_track.hpp"

using namespace client;

own_media::own_media(own_audio &own_audio_) : own_audio_(own_audio_) {}

rtc::google::audio_source *own_media::get_audio() const {
  auto track = own_audio_.get_track();
  if (!track)
    return nullptr;
  return &track->get_source();
}
