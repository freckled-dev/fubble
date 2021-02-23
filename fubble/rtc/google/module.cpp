#include "module.hpp"
#include "fubble/rtc/google/asio_signaling_thread.hpp"
#include "fubble/rtc/google/capture/audio/device.hpp"
#include "fubble/rtc/google/capture/audio/device_creator.hpp"
#include "fubble/rtc/google/capture/video/device.hpp"
#include "fubble/rtc/google/factory.hpp"

using namespace rtc::google;

module::module(std::shared_ptr<utils::executor_module> executor_module,
               const settings rtc_settings)
    : executor_module{executor_module}, rtc_settings{rtc_settings} {}

module::~module() = default;

std::shared_ptr<asio_signaling_thread> module::get_asio_signaling_thread() {
  if (!asio_signaling_thread_)
    asio_signaling_thread_ = std::make_shared<asio_signaling_thread>(
        *executor_module->get_io_context());
  return asio_signaling_thread_;
}

std::shared_ptr<rtc::factory> module::get_factory() {
  if (!factory_)
    factory_ = std::make_shared<rtc::google::factory>(
        rtc_settings, get_asio_signaling_thread()->get_native());
  return factory_;
}

std::shared_ptr<capture::audio::device_creator>
module::get_audio_device_creator() {
  if (!audio_device_creator)
    audio_device_creator = std::make_shared<capture::audio::device_creator>(
        *std::static_pointer_cast<rtc::google::factory>(get_factory()));
  return audio_device_creator;
}

std::shared_ptr<capture::audio::device> module::get_audio_device() {
  if (!audio_device)
    audio_device = get_audio_device_creator()->create();
  return audio_device;
}

std::shared_ptr<capture::video::device_factory>
module::get_video_device_creator() {
  if (!video_device_creator)
    video_device_creator = std::make_shared<capture::video::device_factory>();
  return video_device_creator;
}
