#ifndef RTC_GOOGLE_VIDEO_TRACK_ADAPTER_HPP
#define RTC_GOOGLE_VIDEO_TRACK_ADAPTER_HPP

#include "rtc/google/capture/video/device.hpp"
#include <media/base/adapted_video_track_source.h>

namespace rtc::google {
class video_track_source {
public:
  video_track_source();
  ~video_track_source();
  void handle_frame(const webrtc::VideoFrame &frame);

  class adapter : public rtc::AdaptedVideoTrackSource {
  public:
    SourceState state() const override;
    bool remote() const override;
    bool is_screencast() const override;
    absl::optional<bool> needs_denoising() const override;
    void handle_frame(const webrtc::VideoFrame &frame);
  };
  rtc::scoped_refptr<adapter> source_adapter() const;

protected:
  const rtc::scoped_refptr<adapter> adapter_;
  const std::shared_ptr<capture::video::device> source_device;
};
} // namespace rtc::google

#endif
