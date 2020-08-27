#include "own_media.hpp"
#include "loopback_audio.hpp"
#include "own_audio_track.hpp"
#include "rtc/google/audio_track.hpp"

using namespace client;

own_media::own_media(own_audio_track &own_audio_track_, own_video &own_video_)
    : videos(own_video_), own_audio_track_(own_audio_track_) {}

rtc::google::audio_track *own_media::get_audio() const {
  auto track = own_audio_track_.get_track();
  return track.get();
}

own_video &own_media::get_videos() const { return videos; }
