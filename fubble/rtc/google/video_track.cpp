#include "video_track.hpp"
#include "video_track_source.hpp"

using namespace rtc::google;

video_track::video_track(
    const rtc::scoped_refptr<webrtc::VideoTrackInterface> track_)
    : google::track(track_) {}

video_track::~video_track() {}

// TODO gross!
void video_track::set_enabled(bool enabled) {
  google::track::set_enabled(enabled);
}
