#include "video_settings.hpp"
#include <fmt/format.h>
#include <fubble/client/add_video_to_connection.hpp>
#include <fubble/client/own_media.hpp>
#include <fubble/client/tracks_adder.hpp>
#include <fubble/rtc/google/capture/video/device.hpp>
#include <fubble/rtc/google/video_source.hpp>

using namespace client;

video_settings::video_settings(
    rtc::video_devices &enumerator,
    std::shared_ptr<rtc::video_device_factory> device_creator,
    own_video &own_media_, tracks_adder &tracks_adder_,
    add_video_to_connection_factory &add_video_to_connection_factory_,
    const rtc::video::capability &capability_)
    : enumerator(enumerator), device_creator(device_creator),
      own_media_(own_media_), tracks_adder_(tracks_adder_),
      add_video_to_connection_factory_(add_video_to_connection_factory_),
      capability{capability_} {
  enumerator.on_enumerated_changed.connect(
      [this] { on_video_devices_changed(); });
  setup_initial_device();
}

video_settings::~video_settings() = default;

void video_settings::setup_initial_device() {
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

void video_settings::pause(bool paused_) {
  if (paused == paused_)
    return;
  paused = paused_;
  if (paused) {
    reset_current_video_capture();
    on_video_source_changed();
  } else {
    if (!last_device_id) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << "cannot unpause a video without a device_id set!";
      return;
    }
    change_to_device(last_device_id.value());
  }
  on_paused(paused);
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
  last_device_id = id;
  if (paused) {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "cancelling device change due to paused";
    return;
  }
  try {
    std::shared_ptr<rtc::video_device> capture_device_check =
        device_creator->create(id);
    capture_device_check->start(capability);
    capture_device = capture_device_check;
  } catch (...) {
    last_device_id.reset();
    on_video_source_changed();
    boost::rethrow_exception(boost::current_exception());
  }
  video_track_adder =
      add_video_to_connection_factory_.create(capture_device->get_source());
  tracks_adder_.add(video_track_adder);
  own_media_.add(std::dynamic_pointer_cast<rtc::google::video_source>(
      capture_device->get_source()));
  on_video_source_changed();
}

void video_settings::reset_current_video_capture() {
  if (!capture_device)
    return;
  tracks_adder_.remove(video_track_adder);
  video_track_adder.reset();
  own_media_.remove(std::dynamic_pointer_cast<rtc::google::video_source>(
      capture_device->get_source()));
  capture_device->stop();
  capture_device.reset();
}

void video_settings::reset_current_video() {
  reset_current_video_capture();
  last_device_id.reset();
  on_video_source_changed();
}

void video_settings::on_video_devices_changed() {
  if (!is_a_video_available())
    return;
  auto devices = enumerator.get_enumerated();
  auto found =
      std::find_if(devices.cbegin(), devices.cend(), [&](const auto &check) {
        return check.id == last_device_id.value();
      });
  if (found != devices.cend())
    return;
  // current device is no more
  reset_current_video();
}

std::shared_ptr<rtc::video_source> video_settings::get_video_source() const {
  return capture_device->get_source();
}

bool video_settings::is_a_video_available() const {
  return last_device_id.has_value();
}

boost::optional<std::string> video_settings::get_device_id() const {
  return last_device_id;
}
