#include "factory.hpp"
#include "connection.hpp"
#include "video_track.hpp"

using namespace rtc::gstreamer;

factory::factory(std::shared_ptr<boost::executor> executor)
    : executor{executor} {}

std::unique_ptr<rtc::connection> factory::create_connection() {
  return std::make_unique<connection>(*executor);
}

std::unique_ptr<rtc::audio_track>
factory::create_audio_track(rtc::audio_source &) {
  BOOST_ASSERT(false && "not implemented");
  return nullptr;
}

std::unique_ptr<rtc::video_track> factory::create_video_track(
    [[maybe_unused]] const std::shared_ptr<rtc::video_source> &source) {
  BOOST_ASSERT(std::dynamic_pointer_cast<video_source>(source));
  return std::make_unique<video_track>();
}

std::shared_ptr<rtc::audio_devices> factory::get_audio_devices() { return {}; }
