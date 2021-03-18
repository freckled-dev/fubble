#include "module.hpp"
#include "rpi-webrtc-streamer/src/raspi_encoder.h"
#include "rpi-webrtc-streamer/src/config_media.h"
#include <api/video_codecs/builtin_video_encoder_factory.h>

using namespace fubble::rpi_h264_encoder;

video_encoder_factory_factory::video_encoder_factory_factory(
    const config &config_)
    : config_{config_} {}

std::unique_ptr<webrtc::VideoEncoderFactory>
video_encoder_factory_factory::create() {
  auto config_media_ = ConfigMediaSingleton::Instance();
  config_media_->SetVideoDynamicFps(true);
  config_media_->SetVideoDynamicResolution(true);
  config_media_->SetResolution4_3(false);
  // config_media_->SetVideoResolutionList169("1920x1080");
  //config_media_->SetFixedVideoFps(30);
  //config_media_->SetFixedVideoResolution("1920x1080");
  //config_media_->SetMaxBitrate(10'000'000);

  return webrtc::CreateRaspiVideoEncoderFactory();
}
