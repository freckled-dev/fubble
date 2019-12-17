#include "video_track.hpp"
#include "video_track_source.hpp"

using namespace rtc::google;

video_track::video_track(
    const rtc::scoped_refptr<webrtc::VideoTrackInterface> &track)
    : track(track) {}

video_track::~video_track() {}

rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
video_track::native_track() const {
  return track;
}
