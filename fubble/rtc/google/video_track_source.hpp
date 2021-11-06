#ifndef RTC_GOOGLE_VIDEO_TRACK_SOURCE_HPP
#define RTC_GOOGLE_VIDEO_TRACK_SOURCE_HPP

#include "video_source.hpp"
#include "video_track.hpp"
#include <fubble/rtc/google/video_track_source_adapter.hpp>
#include <fubble/utils/export.hpp>

namespace rtc {
namespace google {
class FUBBLE_PUBLIC video_track_source : public video_track {
public:
  video_track_source(
      const rtc::scoped_refptr<webrtc::VideoTrackInterface> &track,
      const rtc::scoped_refptr<video_track_source_adapter> &source_adapter,
      const std::shared_ptr<video_source> &source);
  ~video_track_source();

  rtc::scoped_refptr<video_track_source_adapter> source_adapter() const;

  void set_content_hint(const content_hint hint) override;

  void set_enabled(bool enabled) override;

  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
  native_track() const override;

protected:
  void handle_frame(const rtc::video_frame &frame);

  const rtc::scoped_refptr<video_track_source_adapter> adapter_;
  const std::shared_ptr<video_source> source;
  const rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track_;
  sigslot::scoped_connection on_frame_connection;
}; // namespace google
} // namespace google
} // namespace rtc

#endif
