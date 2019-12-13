#include "video_track.hpp"
#include "video_track_source.hpp"

using namespace rtc::google;

video_track::video_track(
    const rtc::scoped_refptr<webrtc::VideoTrackInterface> &track,
    const std::shared_ptr<video_track_source> &source)
    : track(track), source(source) {}

video_track::~video_track() = default;

rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
video_track::native_track() const {
  return track;
}
