#include "module.hpp"
#include <fubble/rtc/google/asio_signaling_thread.hpp>
#include <fubble/rtc/google/capture/audio/device.hpp>
#include <fubble/rtc/google/capture/audio/device_creator.hpp>
#include <fubble/rtc/google/capture/video/device.hpp>
#include <fubble/rtc/google/factory.hpp>
#include <fubble/rtc/google/video_encoder_factory_factory.hpp>
#include <fubble/rtc/video_device_factory.hpp>

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
        get_video_encoder_factory_factory(), rtc_settings,
        get_asio_signaling_thread()->get_native());
  return factory_;
}

std::shared_ptr<capture::audio::device_creator>
module::get_audio_device_creator() {
  if (!audio_device_creator)
    audio_device_creator = std::make_shared<capture::audio::device_creator>(
        *std::dynamic_pointer_cast<rtc::google::factory>(get_factory()));
  return audio_device_creator;
}

std::shared_ptr<rtc::audio_device> module::get_audio_device() {
  if (!audio_device_)
    audio_device_ = get_audio_device_creator()->create();
  return audio_device_;
}

std::shared_ptr<rtc::video_device_factory> module::get_video_device_creator() {
  if (!video_device_creator)
    video_device_creator = std::make_shared<capture::video::device_factory>();
  return video_device_creator;
}

std::shared_ptr<rtc::google::video_encoder_factory_factory>
module::get_video_encoder_factory_factory() {
  if (!video_encoder_factory_factory_)
    video_encoder_factory_factory_ =
        std::make_shared<video_encoder_factory_factory>();
  return video_encoder_factory_factory_;
}
