#ifndef UUID_416EAA6D_68BD_48A5_8D2D_524EA9806502
#define UUID_416EAA6D_68BD_48A5_8D2D_524EA9806502

#include <media/base/adapted_video_track_source.h>

namespace rtc {
namespace google {
class video_track_source_adapter : public rtc::AdaptedVideoTrackSource {
public:
  SourceState state() const override;
  bool remote() const override;
  bool is_screencast() const override;
  absl::optional<bool> needs_denoising() const override;

  void handle_frame(const webrtc::VideoFrame &frame);
};
} // namespace google
} // namespace rtc

#endif
