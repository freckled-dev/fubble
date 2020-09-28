#include "device.hpp"
#include "exception.hpp"
#include <api/video/video_sink_interface.h>
#include <modules/audio_device/include/audio_device.h>
#include <modules/video_capture/video_capture.h>
#include <modules/video_capture/video_capture_factory.h>

using namespace rtc::google::capture::video;

namespace {
struct could_not_instance_device : utils::exception {};
struct could_not_start_device : utils::exception {};
using device_id_info = boost::error_info<struct device_id_tag, std::string>;

class device_sink_source_adapter final
    : public rtc::VideoSinkInterface<webrtc::VideoFrame>,
      public rtc::google::video_source {
public:
  void OnFrame(const webrtc::VideoFrame &frame) override {
#if 0
  BOOST_LOG_SEV(logger, logging::severity::debug) << "OnFrame()";
#endif
    on_frame(frame);
  }

  void OnDiscardedFrame() override {
    BOOST_LOG_SEV(logger, logging::severity::debug) << "OnDiscardedFrame()";
  }

protected:
  rtc::logger logger{"device_sink_source_adapter"};
};

class device_impl final : public device {
public:
  device_impl(
      const rtc::scoped_refptr<webrtc::VideoCaptureModule> &native_device,
      const std::string &id)
      : id(id), device_(native_device) {
    BOOST_ASSERT(native_device);
    device_->RegisterCaptureDataCallback(source_adapter.get());
  }

  ~device_impl() {
    if (device_->CaptureStarted())
      stop();
    device_->DeRegisterCaptureDataCallback();
  }

  void start() override {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    // webrtc::VideoCaptureCapability capabilities;
    webrtc::VideoCaptureCapability capabilities;
    capabilities.width = 1280;
    capabilities.height = 720;
    capabilities.maxFPS = 30;
    auto result = device_->StartCapture(capabilities);
    if (result == 0)
      return;
    BOOST_THROW_EXCEPTION(could_not_start_device() << device_id_info(id));
  }

  void stop() override {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    device_->StopCapture();
  }

  std::shared_ptr<rtc::google::video_source> get_source() const override {
    return source_adapter;
  }

  bool get_started() const override { return device_->CaptureStarted(); }
  std::string get_id() const override { return id; }

protected:
  rtc::logger logger{"video::device"};
  const std::string id;
  const rtc::scoped_refptr<webrtc::VideoCaptureModule> device_;
  std::shared_ptr<device_sink_source_adapter> source_adapter;
};

// a device shall only be instanced by one!
class device_wrapper : public device {
public:
  device_wrapper(std::shared_ptr<device> delegate,
                 std::shared_ptr<int> start_stop_counter)
      : delegate{delegate}, start_stop_counter{start_stop_counter} {}
  ~device_wrapper() {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__
        << ", start_stop_counter.use_count:" << start_stop_counter.use_count();
    stop();
  }
  std::shared_ptr<rtc::google::video_source> get_source() const override {
    return delegate->get_source();
  }
  void start() override {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", started:" << started;
    if (started)
      return;
    started = true;
    int &counter = *start_stop_counter;
    ++counter;
    if (counter == 1)
      delegate->start();
  }
  void stop() override {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", started:" << started;
    if (!started)
      return;
    started = false;
    int &counter = *start_stop_counter;
    --counter;
    BOOST_ASSERT(counter >= 0);
    if (counter == 0)
      delegate->stop();
  }
  bool get_started() const override { return started; }
  std::string get_id() const override { return delegate->get_id(); }

protected:
  rtc::logger logger{"device_wrapper"};
  std::shared_ptr<device> delegate;
  std::shared_ptr<int> start_stop_counter;
  bool started{};
};
} // namespace

device_factory::device_factory() = default;

std::unique_ptr<device> device_factory::create(const std::string &id) {
  auto found =
      std::find_if(devices.begin(), devices.end(), [&](const auto &check) {
        return check.device_->get_id() == id;
      });
  if (found == devices.end()) {
    auto native_device = webrtc::VideoCaptureFactory::Create(id.c_str());
    if (!native_device)
      BOOST_THROW_EXCEPTION(could_not_instance_device() << device_id_info(id));
    device_with_start_stop_counter add;
    add.device_ = std::make_shared<device_impl>(native_device, id);
    add.counter = std::make_shared<int>();
    devices.push_back(add);
    found = std::prev(devices.end());
  }
  return std::make_unique<device_wrapper>(found->device_, found->counter);
}
