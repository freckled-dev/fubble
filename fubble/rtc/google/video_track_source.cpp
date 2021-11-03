#include "video_track_source.hpp"
#include "video_frame.hpp"
#include "video_track_source_adapter.hpp"
#include <rtc_base/ref_counted_object.h>

using namespace rtc::google;

video_track_source::video_track_source(
    const rtc::scoped_refptr<webrtc::VideoTrackInterface> &track,
    const rtc::scoped_refptr<video_track_source_adapter> &source_adapter,
    const std::shared_ptr<video_source> &source)
    : adapter_(source_adapter), source(source), video_track_{track} {

  BOOST_ASSERT(source);
  on_frame_connection = source->on_frame.connect(
      [this](const auto &frame) { handle_frame(frame); });
}

video_track_source::~video_track_source() = default;

void video_track_source::set_enabled(bool enabled) {
  video_track_->set_enabled(enabled);
}

rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
video_track_source::native_track() const {
  return video_track_;
}

void video_track_source::handle_frame(const rtc::video_frame &frame) {
  auto casted = dynamic_cast<const rtc::google::video_frame *>(&frame);
  BOOST_ASSERT(casted);
  adapter_->handle_frame(casted->native());
}

rtc::scoped_refptr<video_track_source_adapter>
video_track_source::source_adapter() const {
  return adapter_;
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
