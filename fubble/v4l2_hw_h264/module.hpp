#ifndef UUID_877B0E32_D5DA_4ECF_9C25_32E4DB237421
#define UUID_877B0E32_D5DA_4ECF_9C25_32E4DB237421

#include <fubble/rtc/google/capture/video/device.hpp>
#include <fubble/rtc/google/video_encoder_factory_factory.hpp>
#include <fubble/rtc/google/video_source.hpp>
#include <fubble/v4l2_hw_h264/config.hpp>

namespace fubble {
namespace v4l2_hw_h264 {
class video_device_factory
    : public rtc::google::capture::video::device_factory {
public:
  std::unique_ptr<rtc::video_device> create(const std::string &id) override;
};

class video_source : public rtc::google::video_source {
public:
};

class video_device : public rtc::google::capture::video::device {
public:
  video_device(const std::string &id);
  void start(const rtc::video::capability &cap) override;
  void stop() override;
  bool get_started() const override;
  std::shared_ptr<rtc::video_source> get_source() const override;
  std::string get_id() const override;

protected:
  const std::unique_ptr<rtc::video_device> noop;
  std::shared_ptr<video_source> source;
};

class video_encoder_factory_factory
    : public rtc::google::video_encoder_factory_factory {
public:
  video_encoder_factory_factory(const config &config_);
  std::unique_ptr<webrtc::VideoEncoderFactory> create() override;

protected:
  const config config_;
};

} // namespace v4l2_hw_h264
} // namespace fubble

#endif
