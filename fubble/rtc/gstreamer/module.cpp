#include "module.hpp"
#include "factory.hpp"
#include "video_device_factory.hpp"

using namespace rtc::gstreamer;

module::module(std::shared_ptr<utils::executor_module> executor_module,
               const settings rtc_settings)
    : executor_module{executor_module}, rtc_settings{rtc_settings} {}

module::~module() = default;

std::shared_ptr<rtc::factory> module::get_factory() {
  if (!factory_)
    factory_ = std::make_shared<factory>(executor_module->get_boost_executor());
  return factory_;
}

std::shared_ptr<rtc::video_device_factory> module::get_video_device_creator() {
  if (!video_device_factory_)
    video_device_factory_ = std::make_shared<video_device_factory>();
  return video_device_factory_;
}
