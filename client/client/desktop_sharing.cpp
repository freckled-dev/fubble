#include "desktop_sharing.hpp"
#include "add_video_to_connection.hpp"
#include "client/logger.hpp"
#include "client/tracks_adder.hpp"
#include "rtc/google/capture/desktop/enumerator.hpp"
#include "utils/timer.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <chrono>

using namespace client;

namespace {
class desktop_sharing_impl final : public desktop_sharing {
public:
  desktop_sharing_impl(
      const std::shared_ptr<utils::timer_factory> timer_factory,
      const std::shared_ptr<tracks_adder> tracks_adder_,
      const std::shared_ptr<add_video_to_connection_factory>
          add_video_to_connection_factory_,
      const std::shared_ptr<video_settings> video_settings_)
      : timer_factory{timer_factory}, tracks_adder_{tracks_adder_},
        add_video_to_connection_factory_{add_video_to_connection_factory_},
        video_settings_{video_settings_} {
    // TODO if video gets unpause. stop desktop_sharing
    video_settings_->on_paused; // TODO
  }

  void pause_video() {
    BOOST_ASSERT(!did_pause);
    if (!video_settings_->is_a_video_available())
      return;
    if (video_settings_->get_paused())
      return;
    video_settings_->pause(true);
    did_pause = true;
  }

  void unpause_video() {
    if (!did_pause)
      return;
    did_pause = false;
    video_settings_->pause(false);
  }

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
    pause_video();
    BOOST_ASSERT(video_source);
    video_adder = add_video_to_connection_factory_->create(video_source);
    set_capturer->start().then(executor,
                               [this](auto result) { on_stopped(result); });
    tracks_adder_->add(video_adder);
    on_added(get());
  }

  std::shared_ptr<rtc::google::video_source> get() override {
    if (!set_capturer)
      return nullptr;
    return set_capturer->get_capturer().get_source();
  }

  void on_stopped(boost::future<void> &result) {
    try {
      result.get();
    } catch (const boost::exception &error) {
      BOOST_LOG_SEV(logger, logging::severity::warning)
          << __FUNCTION__ << ", error:" << boost::diagnostic_information(error);
    }
    reset_instance();
  }

  void stop_or_reset() {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    BOOST_ASSERT(set_capturer);
    if (!set_capturer)
      return;
    if (set_capturer->get_started())
      set_capturer->stop();
    else
      reset_instance();
  }

  void reset_instance() {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    BOOST_ASSERT(set_capturer);
    if (!set_capturer)
      return;
    BOOST_ASSERT(video_adder);
    on_removed(get());
    tracks_adder_->remove(video_adder);
    set_capturer.reset();
    video_adder.reset();
  }

  void reset() override {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    stop_or_reset();
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
  boost::inline_executor executor;
  std::shared_ptr<utils::timer_factory> timer_factory;
  std::unique_ptr<rtc::google::capture::desktop::enumerator> enumerator =
      rtc::google::capture::desktop::enumerator::create();
  std::shared_ptr<rtc::google::capture::desktop::interval_capturer>
      set_capturer;
  std::shared_ptr<add_video_to_connection> video_adder;
  std::shared_ptr<tracks_adder> tracks_adder_;
  std::shared_ptr<add_video_to_connection_factory>
      add_video_to_connection_factory_;
  std::shared_ptr<video_settings> video_settings_;
  bool did_pause{};
};
class desktop_sharing_noop final : public desktop_sharing {
public:
  void set([[maybe_unused]] std::intptr_t id) {}
  std::shared_ptr<rtc::google::video_source> get() override { return nullptr; }
  void reset() {}
  previews get_screen_previews() { return {}; }
  previews get_window_previews() { return {}; }
};
} // namespace

std::unique_ptr<desktop_sharing> desktop_sharing::create(
    const std::shared_ptr<utils::timer_factory> timer_factory,
    const std::shared_ptr<tracks_adder> tracks_adder_,
    const std::shared_ptr<add_video_to_connection_factory>
        add_video_to_connection_factory_,
    const std::shared_ptr<video_settings> video_settings_) {
  return std::make_unique<desktop_sharing_impl>(
      timer_factory, tracks_adder_, add_video_to_connection_factory_,
      video_settings_);
}

std::unique_ptr<desktop_sharing> desktop_sharing::create_noop() {
  return std::make_unique<desktop_sharing_noop>();
}
