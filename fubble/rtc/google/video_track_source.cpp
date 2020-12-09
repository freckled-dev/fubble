#include "video_track_source.hpp"
#include <rtc_base/ref_counted_object.h>

using namespace rtc::google;

video_track_source::video_track_source(
    const rtc::scoped_refptr<webrtc::VideoTrackInterface> &track,
    const rtc::scoped_refptr<video_track_source::adapter> &source_adapter,
    const std::shared_ptr<video_source> &source)
    : video_track(track), adapter_(source_adapter),
      source(source), video_track_{track} {

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

bool video_track_source::adapter::remote() const { return false; }

bool video_track_source::adapter::is_screencast() const { return false; }

absl::optional<bool> video_track_source::adapter::needs_denoising() const {
  return {};
}

void video_track_source::set_content_hint(const content_hint hint) {
  webrtc::VideoTrackInterface::ContentHint casted =
      webrtc::VideoTrackInterface::ContentHint::kNone;
  switch (hint) {
  case content_hint::detailed:
    casted = webrtc::VideoTrackInterface::ContentHint::kDetailed;
    break;
  case content_hint::fluid:
    casted = webrtc::VideoTrackInterface::ContentHint::kFluid;
    break;
  case content_hint::text:
    casted = webrtc::VideoTrackInterface::ContentHint::kText;
    break;
  case content_hint::none:
    casted = webrtc::VideoTrackInterface::ContentHint::kNone;
    break;
  }
  video_track_->set_content_hint(casted);
}
