#ifndef UUID_C066FD33_993D_450D_9B51_EE1810DE84B9
#define UUID_C066FD33_993D_450D_9B51_EE1810DE84B9

#include <api/video_codecs/video_encoder.h>
#include <fubble/v4l2_hw_h264/config.hpp>

namespace fubble {
namespace v4l2_hw_h264 {
class video_encoder : public webrtc::VideoEncoder {
public:
  static std::unique_ptr<video_encoder> create(const config &config_);
  static std::unique_ptr<video_encoder>
  create_shared(std::shared_ptr<video_encoder> delegate);
};
} // namespace v4l2_hw_h264
} // namespace fubble

#endif
