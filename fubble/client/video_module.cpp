#include "video_module.hpp"
#include "fubble/client/add_video_to_connection.hpp"
#include "fubble/client/own_video.hpp"
#include "fubble/client/video_settings.hpp"
#include "fubble/rtc/google/capture/video/enumerator.hpp"
#include "fubble/utils/timer.hpp"

using namespace client;

video_module::video_module(
    std::shared_ptr<utils::executor_module> executor_module,
    std::shared_ptr<rtc::google::module> rtc_module,
    std::shared_ptr<session_module> session_module_, const config &config_)
    : executor_module{executor_module}, rtc_module{rtc_module},
      session_module_{session_module_}, config_{config_} {}

std::shared_ptr<rtc::video_devices> video_module::get_enumerator() {
  if (!video_enumerator) {
    if (config_.enabled) {
      video_enumerator_google =
          std::make_unique<rtc::google::capture::video::enumerator>();
      video_enumerator_timer = std::make_unique<utils::interval_timer>(
          *executor_module->get_io_context(),
          config_.video_enumerator_refresh_timeout);
      video_enumerator = rtc::video_devices::create_interval_enumerating(
          *video_enumerator_google, *video_enumerator_timer);
    } else {
      video_enumerator = std::make_unique<rtc::video_devices_noop>();
    }
  }
  return video_enumerator;
}

std::shared_ptr<add_video_to_connection_factory>
video_module::get_add_video_to_connection_factory() {
  if (!add_video_to_connection_factory_)
    add_video_to_connection_factory_ =
        std::make_shared<add_video_to_connection_factory_impl>(
            rtc_module->get_factory());
  return add_video_to_connection_factory_;
}

std::shared_ptr<own_video> video_module::get_own_video() {
  if (!own_video_)
    own_video_ = own_video::create();
  return own_video_;
}

std::shared_ptr<video_settings> video_module::get_video_settings() {
  if (!video_settings_)
    video_settings_ = std::make_shared<client::video_settings>(
        *get_enumerator(), *rtc_module->get_video_device_creator(),
        *get_own_video(), *session_module_->get_tracks_adder(),
        *get_add_video_to_connection_factory());
  return video_settings_;
}
