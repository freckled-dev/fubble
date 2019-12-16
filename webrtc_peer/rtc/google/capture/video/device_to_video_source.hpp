#include "device_to_video_source.hpp"

using namespace rtc::google::capture::video;

device_to_video_source::device_to_video_source(
    std::shared_ptr<capture::video::device> device_,
    std::shared_ptr<video_track_source> source)
    : device_(device_), source(source) {
  signal_connection = device_->signal_frame.connect(
      [this](const auto &frame) { handle_frame(frame); });
}

void handle_frame(const webrtc::VideoFrame &frame) {
  source->handle_frame(frame);
}
