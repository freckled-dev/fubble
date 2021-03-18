#include "video_encoder_factory_factory.hpp"
#include <api/video_codecs/builtin_video_encoder_factory.h>

using namespace rtc::google;

std::unique_ptr<webrtc::VideoEncoderFactory>
video_encoder_factory_factory::create() {
  return webrtc::CreateBuiltinVideoEncoderFactory();
}
