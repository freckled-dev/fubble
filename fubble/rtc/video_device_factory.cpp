#include "video_device_factory.hpp"
#include "video_device.hpp"

using namespace rtc;

std::unique_ptr<video_device>
video_device_factory_noop::create(const std::string &id) {
  return video_device_noop::create(id);
}
