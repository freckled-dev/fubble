#ifndef RTC_GOOGLE_CAPTURE_VIDEO_DEVICE_HPP
#define RTC_GOOGLE_CAPTURE_VIDEO_DEVICE_HPP

#include "logging/logger.hpp"
#include <api/video/video_sink_interface.h>
#include <boost/signals2/signal.hpp>
#include <modules/video_capture/video_capture.h>

namespace rtc::google::capture::video {
class device : public rtc::VideoSinkInterface<webrtc::VideoFrame> {
public:
  device(const rtc::scoped_refptr<webrtc::VideoCaptureModule> &native_device);
  boost::signals2::signal<void(const webrtc::VideoFrame &)> signal_frame;

  void start();
  void stop();

protected:
  void OnFrame(const webrtc::VideoFrame &frame) override;
  void OnDiscardedFrame() override;

  logging::logger logger;
  const rtc::scoped_refptr<webrtc::VideoCaptureModule> device_;
};
} // namespace rtc::google::capture::video

#endif
