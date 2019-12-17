#include "enumerator.hpp"
#include <modules/video_capture/video_capture_factory.h>

using namespace rtc::google::capture::video;

enumerator::enumerator() = default;

std::vector<information> enumerator::operator()() {
  std::vector<information> result;
  std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(
      webrtc::VideoCaptureFactory::CreateDeviceInfo());
  if (!info)
    throw std::runtime_error("deviveinfo could not be initialised");
  int num_devices = info->NumberOfDevices();
  for (int index = 0; index < num_devices; ++index) {
    constexpr std::size_t device_name_size =
        webrtc::kVideoCaptureDeviceNameLength;
    char name[device_name_size];
    constexpr std::size_t id_size = webrtc::kVideoCaptureUniqueNameLength;
    char id[id_size];
    info->GetDeviceName(index, name, device_name_size, id, id_size);
    information item;
    item.id = id;
    item.name = name;
    result.emplace_back(item);
  }
  return result;
}
