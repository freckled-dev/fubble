#include "own_media.hpp"
#include "loopback_audio.hpp"
#include "own_audio_track.hpp"
#include "rtc/google/audio_track.hpp"

using namespace client;

own_media::own_media(loopback_audio &own_audio_,
                     own_audio_track &own_audio_track_)
    : own_audio_(own_audio_), own_audio_track_(own_audio_track_) {}

void own_media::add_video(rtc::google::video_source &video) {
  BOOST_ASSERT(std::find_if(videos.cbegin(), videos.cend(), [&](auto check) {
                 return check == &video;
               }) == videos.cend());
  videos.push_back(&video);
  on_video_added(video);
}

void own_media::remove_video(rtc::google::video_source &video) {
  auto found = std::find_if(videos.cbegin(), videos.cend(),
                            [&](auto check) { return check == &video; });
  if (found == videos.cend()) {
    BOOST_ASSERT(false);
    return;
  }
  videos.erase(found);
  on_video_removed(video);
}
rtc::google::audio_track *own_media::get_audio() const {
  auto &track = own_audio_track_.get_track();
  return &track;
}

#if 0
client::loopback_audio &own_media::get_loopback_audio() const {
  return own_audio_;
}
#endif
