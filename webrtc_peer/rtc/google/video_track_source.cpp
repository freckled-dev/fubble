#include "video_track_source.hpp"
#include <rtc_base/ref_counted_object.h>

using namespace rtc::google;

video_track_source::video_track_source(
    const rtc::scoped_refptr<webrtc::VideoTrackInterface> &track,
    const rtc::scoped_refptr<video_track_source::adapter> &source_adapter,
    const std::shared_ptr<video_source> &source)
    : video_track(track), adapter_(source_adapter), source(source) {
  BOOST_ASSERT(source);
  on_frame_connection = source->on_frame.connect(
      [this](const auto &frame) { handle_frame(frame); });
}

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

bool video_track_source::adapter::is_screencast() const { return false; }

absl::optional<bool> video_track_source::adapter::needs_denoising() const {
  return {};
}
