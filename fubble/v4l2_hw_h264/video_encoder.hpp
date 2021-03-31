#ifndef UUID_C066FD33_993D_450D_9B51_EE1810DE84B9
#define UUID_C066FD33_993D_450D_9B51_EE1810DE84B9

#include <api/video_codecs/video_encoder.h>
#include <fubble/v4l2_hw_h264/config.hpp>
#include <fubble/v4l2_hw_h264/reader.hpp>

namespace fubble {
namespace v4l2_hw_h264 {
class video_encoder : public webrtc::VideoEncoder {
public:
  static std::unique_ptr<video_encoder> create(std::unique_ptr<reader> reader_);
};
} // namespace v4l2_hw_h264
} // namespace fubble

#endif
