#include "device.hpp"
#include <api/media_stream_interface.h>
#include <fubble/rtc/google/audio_source.hpp>

using namespace rtc::google::capture::audio;

device::device(const native_ptr &native) : native(native) {
  source = std::make_unique<audio_source>(*native);
}

device::~device() = default;

rtc::audio_source &device::get_source() { return *source; }
