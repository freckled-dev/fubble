#include "video_track.hpp"
#include "video_track_source.hpp"

using namespace rtc::google;

video_track::video_track(
    const rtc::scoped_refptr<webrtc::VideoTrackInterface> track_)
    : track(track_) {}

video_track::~video_track() {}
