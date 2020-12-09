#include "desktop_sharing_module.hpp"
#include "client/desktop_sharing.hpp"

using namespace client;

desktop_sharing_module::desktop_sharing_module(
    std::shared_ptr<utils::executor_module> executor_module,
    std::shared_ptr<session_module> session_module_,
    std::shared_ptr<video_module> video_module_)
    : executor_module{executor_module}, session_module_{session_module_},
      video_module_{video_module_} {}

std::shared_ptr<desktop_sharing> desktop_sharing_module::get_desktop_sharing() {
  if (!desktop_sharing_)
    desktop_sharing_ = desktop_sharing::create(
        executor_module->get_timer_factory(),
        session_module_->get_tracks_adder(),
        video_module_->get_add_video_to_connection_factory(),
        video_module_->get_video_settings(), session_module_->get_leaver());
  return desktop_sharing_;
}

std::shared_ptr<desktop_sharing_previews>
desktop_sharing_module::get_desktop_sharing_previews() {
  if (!desktop_sharing_previews_)
    desktop_sharing_previews_ =
        desktop_sharing_previews::create(executor_module->get_timer_factory());
  return desktop_sharing_previews_;
}
