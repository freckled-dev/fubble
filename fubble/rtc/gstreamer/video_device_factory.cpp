#include "video_device_factory.hpp"
#include "video_device.hpp"

using namespace rtc::gstreamer;

video_device_factory::~video_device_factory() = default;

std::unique_ptr<rtc::video_device>
video_device_factory::create([[maybe_unused]] const std::string &id) {
  return std::make_unique<rtc::gstreamer::video_device>();
}
