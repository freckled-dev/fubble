#ifndef RTC_GOOGLE_VIDEO_TRACK_SINK_HPP
#define RTC_GOOGLE_VIDEO_TRACK_SINK_HPP

#include "fubble/rtc/logger.hpp"
#include "video_source.hpp"
#include "video_track.hpp"
#include <api/media_stream_interface.h>
#include <api/video/video_frame.h>
#include <api/video/video_sink_interface.h>
#include <boost/signals2/signal.hpp>

namespace rtc {
namespace google {
class video_track_sink : public rtc::VideoSinkInterface<webrtc::VideoFrame>,
                         public video_source, // TODO refactor to `get_source`
                         public video_track {
public:
  video_track_sink(
      const rtc::scoped_refptr<webrtc::VideoTrackInterface> &track);
  ~video_track_sink();

  void set_enabled(bool enabled) override;
  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
  native_track() const override;

protected:
  void OnFrame(const webrtc::VideoFrame &frame) override;
  void OnDiscardedFrame() override;

  rtc::logger logger{"video_track_sink"};
  const rtc::scoped_refptr<webrtc::VideoTrackInterface> native_track_;
  rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track_delegate;
};
} // namespace google
} // namespace rtc

#endif
