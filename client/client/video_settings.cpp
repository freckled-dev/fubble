#include "video_settings.hpp"
#include "client/add_video_to_connection.hpp"
#include "client/own_media.hpp"
#include "client/tracks_adder.hpp"
#include "rtc/google/capture/video/device.hpp"
#include <fmt/format.h>

using namespace client;

video_settings::video_settings(
    rtc::video_devices &enumerator,
    rtc::google::capture::video::device_factory &device_creator,
    own_media &own_media_, tracks_adder &tracks_adder_,
    add_video_to_connection_factory &add_video_to_connection_factory_)
    : enumerator(enumerator), device_creator(device_creator),
      own_media_(own_media_), tracks_adder_(tracks_adder_),
      add_video_to_connection_factory_(add_video_to_connection_factory_) {
  enumerator.enumerate();
  auto devices = enumerator.get_enumerated();
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

void video_settings::pause(bool paused_) {
  if (paused == paused_) {
    BOOST_ASSERT(false);
    return;
  }
  paused = paused_;
  if (paused) {
    reset_current_video();
    on_video_source_changed();
  } else {
    if (!last_device_id) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << "cannot unpause a video without a device_id set!";
      BOOST_ASSERT(false);
      return;
    }
    change_to_device(last_device_id.value());
  }
}

bool video_settings::get_paused() const { return paused; }

void video_settings::change_to_device(const std::string &id) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", id:" << id;
  if (capture_device) {
    if (capture_device->get_id() == id) {
      BOOST_LOG_SEV(logger, logging::severity::debug)
          << "skipping switch because same id";
      return;
    }
    reset_current_video();
  }
  error = false;
  last_device_id = id;
  if (paused) {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "cancelling device change due to paused";
    return;
  }
  try {
    std::shared_ptr<rtc::google::capture::video::device> capture_device_check =
        device_creator.create(id);
    capture_device_check->start();
    capture_device = capture_device_check;
  } catch (...) {
    error = true;
    last_device_id.reset();
    on_video_source_changed();
    boost::rethrow_exception(boost::current_exception());
  }
  video_track_adder = add_video_to_connection_factory_.create(capture_device);
  tracks_adder_.add(*video_track_adder);
  own_media_.add_video(*capture_device);
  on_video_source_changed();
}

void video_settings::reset_current_video() {
  if (!capture_device)
    return;
  tracks_adder_.remove(*video_track_adder);
  video_track_adder.reset();
  own_media_.remove_video(*capture_device);
  capture_device->stop();
  capture_device.reset();
  error = false;
  on_video_source_changed();
}

rtc::google::video_source *video_settings::get_video_source() const {
  return capture_device.get();
}

bool video_settings::is_a_video_available() const {
  return last_device_id.has_value() && !error;
}

std::optional<std::string> video_settings::get_device_id() const {
  return last_device_id;
}
