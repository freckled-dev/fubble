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
#if 0
    auto capabilitiesCount = info->NumberOfCapabilities(id);
    for (auto capabilityIndex = 0; capabilityIndex < capabilitiesCount;
         ++capabilityIndex) {
      webrtc::VideoCaptureCapability capability;
      info->GetCapability(id, capabilityIndex, capability);
    }
#endif
    information item;
    item.id = id;
    item.name = name;
    result.emplace_back(item);
  }
  return result;
#if 0
  auto device_id = devices.front();
  auto camera = webrtc::VideoCaptureFactory::Create(device_id.c_str());
  camera->RegisterCaptureDataCallback(&camera_observer);
  webrtc::VideoCaptureCapability capabilities;
#if 0
      capabilities.height = 1080;
      capabilities.width = 1920;
      capabilities.maxFPS = 30;
#endif
  camera->StartCapture(capabilities);
#endif
}

