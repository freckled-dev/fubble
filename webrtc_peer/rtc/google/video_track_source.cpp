#include "video_track_source.hpp"
#include <rtc_base/ref_counted_object.h>

using namespace rtc::google;

video_track_source::video_track_source()
    : adapter_{new rtc::RefCountedObject<adapter>()} {}

video_track_source::~video_track_source() = default;

void video_track_source::handle_frame(const webrtc::VideoFrame &frame) {
  adapter_->handle_frame(frame);
}

void video_track_source::adapter::handle_frame(
    const webrtc::VideoFrame &frame) {
  OnFrame(frame);
}

rtc::scoped_refptr<video_track_source::adapter>
video_track_source::source_adapter() const {
  return adapter_;
}

video_track_source::adapter::SourceState
video_track_source::adapter::state() const {
  return webrtc::MediaSourceInterface::kLive;
}

bool video_track_source::adapter::remote() const { return true; }

bool video_track_source::adapter::is_screencast() const { return true; }

absl::optional<bool> video_track_source::adapter::needs_denoising() const {
  return {};
}
