#include "video_device.hpp"

using namespace rtc::gstreamer;

namespace {
class video_source_adapter : public rtc::video_source {
public:
};
} // namespace

video_device::video_device() {
  video_source_ = std::make_shared<video_source_adapter>();
}

video_device::~video_device() = default;

void video_device::start([[maybe_unused]] const rtc::video::capability &cap) {
  //
}

void video_device::stop() {
  //
}

bool video_device::get_started() const { return true; }

std::shared_ptr<rtc::video_source> video_device::get_source() const {
  return video_source_;
}

std::string video_device::get_id() const { return ""; }
