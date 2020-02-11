#include "device.hpp"

using namespace rtc::google ::capture::audio;

device::device(const native_ptr &native) : native(native) {}

webrtc::AudioSourceInterface &device::get_native() const { return *native; }
