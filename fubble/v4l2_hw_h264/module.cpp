#include "module.hpp"
#include "video_encoder.hpp"
#include <api/video_codecs/builtin_video_encoder_factory.h>

using namespace fubble::v4l2_hw_h264;

std::unique_ptr<rtc::video_device>
video_device_factory::create(const std::string &id) {
  return std::make_unique<video_device>(id);
}

video_device::video_device(const std::string &id)
    : noop{rtc::video_device_noop::create(id)},
      source{std::make_shared<video_source>()} {}

void video_device::start(const rtc::video::capability &cap) {
  noop->start(cap);
}

void video_device::stop() { noop->stop(); }

bool video_device::get_started() const { return noop->get_started(); }

std::shared_ptr<rtc::video_source> video_device::get_source() const {
  return source;
}

std::string video_device::get_id() const { return noop->get_id(); }

namespace {
// level-asymmetry-allowed=1 packetization-mode=1 profile-level-id=42001f
webrtc::SdpVideoFormat h264_format("H264"
#if 0
    , {{"level-asymmetry-allowed", "1"},
    {"packetization-mode", "1"}}
#endif
);
class encoder_selector
    : public webrtc::VideoEncoderFactory::EncoderSelectorInterface {
public:
  encoder_selector() {}
  void OnCurrentEncoder(const webrtc::SdpVideoFormat &format) {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    (void)format;
    // TODO
  }

  // Called every time the available bitrate is updated. Should return a
  // non-empty if an encoder switch should be performed.
  absl::optional<webrtc::SdpVideoFormat>
  OnAvailableBitrate(const webrtc::DataRate &rate) {
#if 0
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
#endif
    (void)rate;
    return h264_format;
  }

  // Called if the currently used encoder reports itself as broken. Should
  // return a non-empty if an encoder switch should be performed.
  absl::optional<webrtc::SdpVideoFormat> OnEncoderBroken() {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    return h264_format;
  }

  rtc::logger logger{"encoder_selector"};
};

class video_encoder_factory : public webrtc::VideoEncoderFactory {
public:
  std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() const override {
    BOOST_LOG_SEV(const_cast<video_encoder_factory *>(this)->logger,
                  logging::severity::debug)
        << __FUNCTION__;
    std::vector<webrtc::SdpVideoFormat> result;
    result.push_back(h264_format);
    return result;
  }

  CodecInfo
  QueryVideoEncoder(const webrtc::SdpVideoFormat &format) const override {
    BOOST_LOG_SEV(const_cast<video_encoder_factory *>(this)->logger,
                  logging::severity::debug)
        << __FUNCTION__;
    (void)format;
    CodecInfo result;
    result.has_internal_source = true;
    return result;
  }

  std::unique_ptr<webrtc::VideoEncoder>
  CreateVideoEncoder(const webrtc::SdpVideoFormat &format) override {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    (void)format;
    return video_encoder::create();
  }

  std::unique_ptr<EncoderSelectorInterface>
  GetEncoderSelector() const override {
    BOOST_LOG_SEV(const_cast<video_encoder_factory *>(this)->logger,
                  logging::severity::debug)
        << __FUNCTION__;
    return std::make_unique<encoder_selector>();
  }

  rtc::logger logger{"video_encoder_factory"};
};
} // namespace
