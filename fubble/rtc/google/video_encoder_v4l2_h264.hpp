#ifndef UUID_C066FD33_993D_450D_9B51_EE1810DE84B9
#define UUID_C066FD33_993D_450D_9B51_EE1810DE84B9

#include <api/video_codecs/video_encoder.h>

namespace rtc {
namespace google {
class video_encoder_v4l2_h264 : public webrtc::VideoEncoder {
public:
  static std::unique_ptr<video_encoder_v4l2_h264> create();
};
} // namespace google
} // namespace rtc

#endif
