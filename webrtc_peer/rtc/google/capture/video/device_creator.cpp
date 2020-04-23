#include "device_creator.hpp"
#include "device.hpp"
#include "exception.hpp"
#include <boost/exception/all.hpp>
#include <modules/video_capture/video_capture_factory.h>

using namespace rtc::google::capture::video;

namespace {
struct could_not_instance_device : utils::exception {};
using device_id_info = boost::error_info<struct device_id_tag, std::string>;
} // namespace

device_creator::device_creator() = default;

std::unique_ptr<device> device_creator::operator()(const std::string &id) {
  auto native_device = webrtc::VideoCaptureFactory::Create(id.c_str());
  if (!native_device)
    BOOST_THROW_EXCEPTION(could_not_instance_device() << device_id_info(id));
  return std::make_unique<device>(native_device);
}
