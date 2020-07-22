#include "device.hpp"
#include "exception.hpp"
#include <api/video/video_sink_interface.h>
#include <boost/exception/all.hpp>
#include <modules/audio_device/include/audio_device.h>
#include <modules/video_capture/video_capture.h>
#include <modules/video_capture/video_capture_factory.h>

using namespace rtc::google::capture::video;

namespace {
struct could_not_instance_device : utils::exception {};
using device_id_info = boost::error_info<struct device_id_tag, std::string>;

// TODO move `VideoSinkInterface` into `video_source`
class device_impl : public rtc::VideoSinkInterface<webrtc::VideoFrame>,
                    public device {
public:
  device_impl(
      const rtc::scoped_refptr<webrtc::VideoCaptureModule> &native_device,
      const std::string &id)
      : id(id), device_(native_device) {
    BOOST_ASSERT(native_device);
    device_->RegisterCaptureDataCallback(this);
  }

  ~device_impl() {
    if (device_->CaptureStarted())
      stop();
    device_->DeRegisterCaptureDataCallback();
  }

  void start() override {
    BOOST_LOG_SEV(logger, logging::severity::debug) << "start()";
    // webrtc::VideoCaptureCapability capabilities;
    webrtc::VideoCaptureCapability capabilities;
    capabilities.width = 1280;
    capabilities.height = 720;
    capabilities.maxFPS = 30;
    auto result = device_->StartCapture(capabilities);
    if (result == 0)
      return;
    throw std::runtime_error("could not start recording");
  }

  void stop() override {
    BOOST_LOG_SEV(logger, logging::severity::debug) << "stop()";
    device_->StopCapture();
  }

  void OnFrame(const webrtc::VideoFrame &frame) override {
#if 0
  BOOST_LOG_SEV(logger, logging::severity::debug) << "OnFrame()";
#endif
    on_frame(frame);
  }

  void OnDiscardedFrame() override {
    BOOST_LOG_SEV(logger, logging::severity::debug) << "OnDiscardedFrame()";
  }

  std::string get_id() const override { return id; }

protected:
  rtc::logger logger{"video::device"};
  const std::string id;
  const rtc::scoped_refptr<webrtc::VideoCaptureModule> device_;
};
} // namespace

device_factory::device_factory() = default;

std::unique_ptr<device> device_factory::create(const std::string &id) {
  auto native_device = webrtc::VideoCaptureFactory::Create(id.c_str());
  if (!native_device)
    BOOST_THROW_EXCEPTION(could_not_instance_device() << device_id_info(id));
  return std::make_unique<device_impl>(native_device, id);
}
