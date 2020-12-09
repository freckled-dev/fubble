#include "device.hpp"
#include "rtc/google/audio_source.hpp"

using namespace rtc::google::capture::audio;

device::device(const native_ptr &native) : native(native) {
  source = std::make_unique<audio_source>(*native);
}

device::~device() = default;

rtc::google::audio_source &device::get_source() { return *source; }
