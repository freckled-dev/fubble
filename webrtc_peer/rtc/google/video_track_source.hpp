#ifndef RTC_GOOGLE_VIDEO_TRACK_ADAPTER_HPP
#define RTC_GOOGLE_VIDEO_TRACK_ADAPTER_HPP

#include <media/base/adapted_video_track_source.h>

namespace rtc::google {
class video_track_source {
public:
  video_track_source();

  class adapter : public rtc::AdaptedVideoTrackSource {
  public:
    SourceState state() const override;
    bool remote() const override;
    bool is_screencast() const override;
    absl::optional<bool> needs_denoising() const override;
  };
  rtc::scoped_refptr<adapter> source_adapter() const;

protected:
  rtc::scoped_refptr<adapter> adapter_;
};
} // namespace rtc::google

#endif
