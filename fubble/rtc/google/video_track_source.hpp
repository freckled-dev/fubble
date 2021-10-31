#ifndef RTC_GOOGLE_VIDEO_TRACK_SOURCE_HPP
#define RTC_GOOGLE_VIDEO_TRACK_SOURCE_HPP

#include "video_source.hpp"
#include "video_track.hpp"
#include <fubble/utils/export.hpp>
#include <media/base/adapted_video_track_source.h>

namespace rtc {
namespace google {
class FUBBLE_PUBLIC video_track_source : public video_track {
public:
  class adapter : public rtc::AdaptedVideoTrackSource {
  public:
    SourceState state() const override;
    bool remote() const override;
    bool is_screencast() const override;
    absl::optional<bool> needs_denoising() const override;
    void handle_frame(const webrtc::VideoFrame &frame);
  };

  video_track_source(
      const rtc::scoped_refptr<webrtc::VideoTrackInterface> &track,
      const rtc::scoped_refptr<adapter> &source_adapter,
      const std::shared_ptr<video_source> &source);
  ~video_track_source();

  rtc::scoped_refptr<adapter> source_adapter() const;

  enum class content_hint { none, fluid, detailed, text };
  void set_content_hint(const content_hint hint);

protected:
  void handle_frame(const webrtc::VideoFrame &frame);

  const rtc::scoped_refptr<adapter> adapter_;
  const std::shared_ptr<video_source> source;
  const rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track_;
  boost::signals2::scoped_connection on_frame_connection;
}; // namespace google
} // namespace google
} // namespace rtc

#endif
