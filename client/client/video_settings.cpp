#include "video_settings.hpp"
#include "client/add_video_to_connection.hpp"
#include "client/own_media.hpp"
#include "client/tracks_adder.hpp"
#include "rtc/google/capture/video/device.hpp"
#include "rtc/google/capture/video/device_creator.hpp"
#include "rtc/google/capture/video/enumerator.hpp"
#include <fmt/format.h>

using namespace client;

video_settings::video_settings(
    rtc::google::capture::video::enumerator &enumerator,
    rtc::google::capture::video::device_creator &device_creator,
    own_media &own_media_, tracks_adder &tracks_adder_,
    add_video_to_connection_factory &add_video_to_connection_factory_)
    : enumerator(enumerator), device_creator(device_creator),
      own_media_(own_media_), tracks_adder_(tracks_adder_),
      add_video_to_connection_factory_(add_video_to_connection_factory_) {
  auto devices = enumerator.enumerate();
  for (const auto &device : devices)
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "capture device, name:" << device.name << ", id:" << device.id;
  if (devices.empty())
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << "there are no capture devices";

  for (const auto &current_device : devices) {
    try {
      change_to_device(current_device.id);
      break;
    } catch (const std::exception &error) {
      BOOST_LOG_SEV(logger, logging::severity::warning) << fmt::format(
          "could not start capturing from device, id:'{}' error:{}",
          current_device.id, error.what());
    }
  }
  if (!capture_device) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << "no capture device could be initialsed";
  } else {
  }
}

video_settings::~video_settings() = default;

void video_settings::disable_video() {
  capture_device.reset();
  on_video_source_changed();
}

void video_settings::change_to_device(const std::string &id) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", id:" << id;
  if (capture_device) {
    if (capture_device->get_id() == id) {
      BOOST_LOG_SEV(logger, logging::severity::debug)
          << "skipping switch because same id";
      return;
    }
    own_media_.remove_video(*capture_device);
    capture_device->stop();
    capture_device.reset();
  }
  std::shared_ptr<rtc::google::capture::video::device> capture_device_check =
      device_creator.create(id);
  capture_device_check->start();
  capture_device = capture_device_check;
  video_track_adder = add_video_to_connection_factory_.create(capture_device);
  tracks_adder_.add(*video_track_adder);
  own_media_.add_video(*capture_device);
  on_video_source_changed();
}

rtc::google::video_source *video_settings::get_video_source() const {
  return capture_device.get();
}
