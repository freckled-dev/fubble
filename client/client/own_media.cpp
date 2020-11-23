#include "own_media.hpp"
#include "client/desktop_sharing.hpp"
#include "loopback_audio.hpp"
#include "own_audio_track.hpp"
#include "rtc/google/audio_track.hpp"

using namespace client;

own_media::own_media() = default;
own_media::~own_media() = default;

void own_media::set_own_audio_track(
    const std::shared_ptr<own_audio_track> &own_audio_track_parameter) {
  own_audio_track_ = own_audio_track_parameter;
}

void own_media::set_own_video(
    const std::shared_ptr<own_video> &own_video_parameter) {
  own_video_ = own_video_parameter;
}

rtc::google::audio_track *own_media::get_audio() const {
  auto track = own_audio_track_->get_track();
  return track.get();
}

std::shared_ptr<own_video> own_media::get_videos() const { return own_video_; }

std::shared_ptr<desktop_sharing> own_media::get_desktop_sharing() const {
  return desktop_sharing_;
}

void own_media::set_desktop_sharing(
    std::shared_ptr<desktop_sharing> desktop_sharing_parameter) {
  desktop_sharing_ = desktop_sharing_parameter;
}
