#ifndef RTC_GOOGLE_CAPTURE_VIDEO_DEVICE_HPP
#define RTC_GOOGLE_CAPTURE_VIDEO_DEVICE_HPP

#include "rtc/google/video_source.hpp"
#include "rtc/logger.hpp"
#include <api/video/video_sink_interface.h>
#include <boost/signals2/signal.hpp>
#include <modules/video_capture/video_capture.h>

namespace rtc {
namespace google {
namespace capture {
namespace video {
// TODO move `VideoSinkInterface` into `video_source`
class device : public rtc::VideoSinkInterface<webrtc::VideoFrame>,
               public video_source {
public:
  device(const rtc::scoped_refptr<webrtc::VideoCaptureModule> &native_device,
         const std::string &id);
  ~device();

  void start();
  void stop();

  std::string get_id() const { return id; }

protected:
  void OnFrame(const webrtc::VideoFrame &frame) override;
  void OnDiscardedFrame() override;

  class logger logger {
    "video::device"
  };
  const std::string id;
  const rtc::scoped_refptr<webrtc::VideoCaptureModule> device_;
};
} // namespace video
} // namespace capture
} // namespace google
} // namespace rtc

#endif
