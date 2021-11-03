#include "video_track_source_adapter.hpp"

using namespace rtc::google;

void video_track_source_adapter::handle_frame(const webrtc::VideoFrame &frame) {
  OnFrame(frame);
}

video_track_source_adapter::SourceState
video_track_source_adapter::state() const {
  return webrtc::MediaSourceInterface::kLive;
}

bool video_track_source_adapter::remote() const { return false; }

bool video_track_source_adapter::is_screencast() const { return false; }

absl::optional<bool> video_track_source_adapter::needs_denoising() const {
  return {};
}
