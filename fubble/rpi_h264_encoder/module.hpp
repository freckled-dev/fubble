#ifndef UUID_33C42A55_7BC2_4995_B3B9_2FAABEC53F81
#define UUID_33C42A55_7BC2_4995_B3B9_2FAABEC53F81

#include <fubble/rtc/google/capture/video/device.hpp>
#include <fubble/rtc/google/video_encoder_factory_factory.hpp>
#include <fubble/rtc/google/video_source.hpp>
#include <fubble/v4l2_hw_h264/config.hpp>

namespace fubble {
namespace rpi_h264_encoder {

struct config {};
class video_encoder_factory_factory
    : public rtc::google::video_encoder_factory_factory {
public:
  video_encoder_factory_factory(const config &config_);
  std::unique_ptr<webrtc::VideoEncoderFactory> create() override;

protected:
  const config config_;
};

} // namespace rpi_h264_encoder
} // namespace fubble

#endif
