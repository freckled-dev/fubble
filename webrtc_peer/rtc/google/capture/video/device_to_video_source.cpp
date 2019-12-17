#include "device_to_video_source.hpp"

#if 0
using namespace rtc::google::capture::video;

device_to_video_source::device_to_video_source(
    std::shared_ptr<capture::video::device> device_,
    std::shared_ptr<video_track_source> source)
    : device_(device_), source(source) {
  signal_connection = device_->on_frame.connect(
      [this](const auto &frame) { handle_frame(frame); });
}

void device_to_video_source::handle_frame(
    const webrtc::VideoFrame & /*frame*/) {
#if 0
  source->handle_frame(frame);
#endif
}

#endif

