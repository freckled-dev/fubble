#include "device_creator.hpp"
#include "device.hpp"
#include <modules/video_capture/video_capture_factory.h>

using namespace rtc::google::capture::video;

device_creator::device_creator() = default;

std::unique_ptr<device> device_creator::operator()(const std::string &id) {
  auto native_device = webrtc::VideoCaptureFactory::Create(id.c_str());
  return std::make_unique<device>(native_device);
}

