#ifndef RTC_GOOGLE_VIDEO_TRACK_SINK_HPP
#define RTC_GOOGLE_VIDEO_TRACK_SINK_HPP

#include "logging/logger.hpp"
#include "video_source.hpp"
#include "video_track.hpp"
#include <api/media_stream_interface.h>
#include <api/video/video_frame.h>
#include <api/video/video_sink_interface.h>
#include <boost/signals2/signal.hpp>

namespace rtc::google {
class video_track_sink : public rtc::VideoSinkInterface<webrtc::VideoFrame>,
                         public video_track,
                         public video_source {
public:
  video_track_sink(
      const rtc::scoped_refptr<webrtc::VideoTrackInterface> &track);
  ~video_track_sink();

protected:
  void OnFrame(const webrtc::VideoFrame &frame) override;
  void OnDiscardedFrame() override;

  logging::logger logger;
};
} // namespace rtc::google

#endif
