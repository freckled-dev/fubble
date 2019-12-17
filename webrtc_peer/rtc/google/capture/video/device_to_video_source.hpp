#ifndef RTC_GOOGLE_CAPTURE_VIDEO_DEVICE_TO_VIDEO_SOURCE_HPP
#define RTC_GOOGLE_CAPTURE_VIDEO_DEVICE_TO_VIDEO_SOURCE_HPP

#if 0

#include "rtc/google/capture/video/device.hpp"
#include "rtc/google/video_track_source.hpp"

namespace rtc::google::capture::video {
class device_to_video_source {
public:
  device_to_video_source(std::shared_ptr<capture::video::device> device_,
                         std::shared_ptr<video_track_source> source);

protected:
  void handle_frame(const webrtc::VideoFrame &frame);

  const std::shared_ptr<capture::video::device> device_;
  const std::shared_ptr<video_track_source> source;
  boost::signals2::scoped_connection signal_connection;
};
} // namespace rtc::google::capture::video

#endif

#endif

