#include "device.hpp"

using namespace rtc::google ::capture::audio;

device::device(const native_ptr &native)
    : audio_source(*native), native(native) {}
