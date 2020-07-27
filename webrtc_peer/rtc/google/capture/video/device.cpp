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
struct could_not_start_device : utils::exception {};
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
    BOOST_THROW_EXCEPTION(could_not_start_device() << device_id_info(id));
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

// a device shall only be instanced by one!
class device_wrapper : public device {
public:
  device_wrapper(std::shared_ptr<device> delegate,
                 std::shared_ptr<int> start_stop_counter)
      : delegate{delegate}, start_stop_counter{start_stop_counter} {
    on_frame_connection = delegate->on_frame.connect(
        [this](const auto &frame) { on_frame(frame); });
  }
  ~device_wrapper() {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__
        << ", start_stop_counter.use_count:" << start_stop_counter.use_count();
    stop();
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
  std::string get_id() const override { return delegate->get_id(); }

protected:
  rtc::logger logger{"device_wrapper"};
  std::shared_ptr<device> delegate;
  std::shared_ptr<int> start_stop_counter;
  bool started{};
  boost::signals2::scoped_connection on_frame_connection;
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
