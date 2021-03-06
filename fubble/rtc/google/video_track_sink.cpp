#include "video_track_sink.hpp"

using namespace rtc::google;

video_track_sink::video_track_sink(
    const rtc::scoped_refptr<webrtc::VideoTrackInterface> &track)
    : video_track(track), video_track_delegate{track} {
  track->AddOrUpdateSink(this, rtc::VideoSinkWants());
}

video_track_sink::~video_track_sink() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  video_track_delegate->RemoveSink(this);
}

void video_track_sink::OnFrame(const webrtc::VideoFrame &frame) {
  on_frame(frame);
}

void video_track_sink::OnDiscardedFrame() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "OnDiscardedFrame()";
}
