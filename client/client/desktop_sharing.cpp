#include "desktop_sharing.hpp"
#include "add_video_to_connection.hpp"
#include "client/logger.hpp"
#include "client/tracks_adder.hpp"
#include "rtc/google/capture/desktop/enumerator.hpp"
#include "utils/timer.hpp"
#include <chrono>

using namespace client;

namespace {
class desktop_sharing_impl final : public desktop_sharing {
public:
  desktop_sharing_impl(std::shared_ptr<utils::timer_factory> timer_factory)
      : timer_factory{timer_factory} {}

  void set(std::intptr_t id) {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", id:" << id;
    if (set_capturer)
      reset();
    BOOST_ASSERT(!set_capturer);
    auto capturer = rtc::google::capture::desktop::capturer::create(id);
    static const std::chrono::steady_clock::duration interval =
        std::chrono::milliseconds(1000 / 40);
    auto timer = timer_factory->create_interval_timer(interval);
    set_capturer = rtc::google::capture::desktop::interval_capturer::create(
        std::move(timer), std::move(capturer));
    auto video_source = set_capturer->get_capturer().get_source();
    BOOST_ASSERT(video_source);
    auto video_adder = add_video_to_connection_factory_->create(video_source);
    // tracks_adder_->add(video_adder);
  }

  void reset() {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    BOOST_ASSERT(set_capturer);
    if (!set_capturer)
      return;
  }

  std::vector<preview> get_screen_previews() {
    return create_previews(desktop_sharing::type::screen);
  }

  std::vector<preview> get_window_previews() {
    return create_previews(desktop_sharing::type::window);
  }

  std::vector<preview> create_previews(desktop_sharing::type type) {
    std::vector<preview> result;
    static const std::chrono::steady_clock::duration preview_interval =
        std::chrono::seconds(1);
    enumerator->enumerate();
    auto enumerated = [&] {
      if (type == desktop_sharing::type::screen)
        return enumerator->get_screens();
      return enumerator->get_windows();
    }();
    std::transform(
        enumerated.cbegin(), enumerated.cend(), std::back_inserter(result),
        [&](auto &enumerated_item) {
          preview result;
          auto capturer = [&] {
            if (type == desktop_sharing::type::screen)
              return rtc::google::capture::desktop::capturer::create_screen(
                  enumerated_item.id);
            return rtc::google::capture::desktop::capturer::create_window(
                enumerated_item.id);
          }();
          auto timer = timer_factory->create_interval_timer(preview_interval);
          auto interval_capturer =
              rtc::google::capture::desktop::interval_capturer::create(
                  std::move(timer), std::move(capturer));
          result.capturer = std::move(interval_capturer);
          return result;
        });
    return result;
  }

protected:
  client::logger logger{"desktop_sharing_impl"};
  std::shared_ptr<utils::timer_factory> timer_factory;
  std::unique_ptr<rtc::google::capture::desktop::enumerator> enumerator =
      rtc::google::capture::desktop::enumerator::create();
  std::shared_ptr<rtc::google::capture::desktop::interval_capturer>
      set_capturer;
  std::shared_ptr<tracks_adder> tracks_adder_;
  std::shared_ptr<add_video_to_connection_factory>
      add_video_to_connection_factory_;
};
} // namespace

std::unique_ptr<desktop_sharing>
desktop_sharing::create(std::shared_ptr<utils::timer_factory> timer_factory) {
  return std::make_unique<desktop_sharing_impl>(timer_factory);
}
