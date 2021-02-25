#include "video_device.hpp"

using namespace rtc::gstreamer;

video_device::~video_device() = default;

void video_device::start(const rtc::video::capability &cap) {}

void video_device::stop() {}

bool video_device::get_started() const {}

std::shared_ptr<rtc::video_source> video_device::get_source() const {}

std::string video_device::get_id() const {}
