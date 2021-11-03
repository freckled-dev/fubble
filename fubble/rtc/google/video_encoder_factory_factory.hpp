#ifndef UUID_8530D5E5_72EF_4E04_BFBE_E3692F551C17
#define UUID_8530D5E5_72EF_4E04_BFBE_E3692F551C17

#include <api/video_codecs/video_encoder_factory.h>
#include <fubble/utils/export.hpp>

namespace rtc {
namespace google {
class FUBBLE_PUBLIC video_encoder_factory_factory {
public:
  virtual ~video_encoder_factory_factory() = default;
  virtual std::unique_ptr<webrtc::VideoEncoderFactory> create();
};
} // namespace google
} // namespace rtc

#endif
