#include "desktop_sharing.hpp"
#include "add_video_to_connection.hpp"
#include "client/logger.hpp"
#include "client/tracks_adder.hpp"
#include "rtc/google/capture/desktop/enumerator.hpp"
#include "rtc/google/video_track_source.hpp"
#include "utils/timer.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <chrono>

using namespace client;

namespace {
class desktop_sharing_previews_impl : public desktop_sharing_previews {
public:
  desktop_sharing_previews_impl(
      const std::shared_ptr<utils::timer_factory> timer_factory)
      : timer_factory{timer_factory}, timer{
                                          timer_factory->create_interval_timer(
                                              std::chrono::seconds(1))} {}

  void enumerate() {
    enumerator->enumerate();
    create_previews(desktop_sharing::type::screen);
    create_previews(desktop_sharing::type::window);
  }

  std::vector<preview> get_screens() override { return screen_previews; }

  std::vector<preview> get_windows() override { return window_previews; }

  void start() override {
    for (auto &preview_ : window_previews)
      start_preview(preview_);
    for (auto &preview_ : screen_previews)
      start_preview(preview_);
    enumerate();
    timer->start([this] { enumerate(); });
  }

  void stop() override {
    timer->stop();
    for (auto &preview_ : window_previews) {
      BOOST_ASSERT(preview_.capturer->get_started());
      preview_.capturer->stop();
    }
    for (auto &preview_ : screen_previews) {
      BOOST_ASSERT(preview_.capturer->get_started());
      preview_.capturer->stop();
    }
  }

  void start_preview(preview preview_) {
    BOOST_ASSERT(!preview_.capturer->get_started());
    preview_.capturer->start().then(executor, [this, preview_](auto result) {
      on_capturer_stopped(preview_, result);
    });
  }

  void on_capturer_stopped(preview preview_, boost::future<void> &result) {
    try {
      result.get();
      return;
    } catch (const boost::exception &error) {
      BOOST_LOG_SEV(logger, logging::severity::info)
          << "preview_ stopped, will remove. id:"
          << preview_.capturer->get_capturer().get_id()
          << ", error:" << boost::diagnostic_information(error);
    }
    auto checker = [&](const preview &check) {
      return preview_.capturer->get_capturer().get_id() ==
             check.capturer->get_capturer().get_id();
    };
    {
      auto found = std::find_if(window_previews.cbegin(),
                                window_previews.cend(), checker);
      if (found != window_previews.cend()) {
        window_previews.erase(found);
        on_window_removed(preview_);
        return;
      }
    }
    {
      auto found = std::find_if(screen_previews.cbegin(),
                                screen_previews.cend(), checker);
      if (found != screen_previews.cend()) {
        screen_previews.erase(found);
        on_screen_removed(preview_);
        return;
      }
    }
  }

  void create_previews(desktop_sharing::type type) {
    std::vector<preview> result;
    auto enumerated = [&] {
      if (type == desktop_sharing::type::screen)
        return enumerator->get_screens();
      return enumerator->get_windows();
    }();
    decltype(enumerated) enumerated_filtered;
    previews *working_previews = [&] {
      if (type == desktop_sharing::type::screen)
        return &screen_previews;
      return &window_previews;
    }();
    auto *on_added = [&] {
      if (type == desktop_sharing::type::screen)
        return &on_screen_added;
      return &on_window_added;
    }();
    // TODO this is a bad algorithm. sort the lists and do adjacent list
    std::copy_if(
        enumerated.cbegin(), enumerated.cend(),
        std::back_inserter(enumerated_filtered), [&](auto check) {
          return working_previews->cend() ==
                 std::find_if(
                     working_previews->cbegin(), working_previews->cend(),
                     [&](const auto &check_preview) {
                       return check_preview.capturer->get_capturer().get_id() ==
                              check.id;
                     });
        });
    if (!enumerated_filtered.empty())
      BOOST_LOG_SEV(logger, logging::severity::debug)
          << "adding:" << enumerated_filtered.size();
    std::transform(enumerated_filtered.cbegin(), enumerated_filtered.cend(),
                   std::back_inserter(*working_previews),
                   [&](auto &enumerated_item) {
                     auto preview_ = create_preview(enumerated_item, type);
                     (*on_added)(preview_);
                     start_preview(preview_);
                     return preview_;
                   });
  }

  preview create_preview(
      rtc::google::capture::desktop::enumerator::information information,
      desktop_sharing::type type) {
    static const std::chrono::steady_clock::duration preview_interval =
        std::chrono::seconds(1);
    preview result;
    auto capturer = [&] {
      if (type == desktop_sharing::type::screen)
        return rtc::google::capture::desktop::capturer::create_screen(
            information.id);
      return rtc::google::capture::desktop::capturer::create_window(
          information.id);
    }();
    auto timer = timer_factory->create_interval_timer(preview_interval);
    auto interval_capturer =
        rtc::google::capture::desktop::interval_capturer::create(
            std::move(timer), std::move(capturer));
    result.capturer = std::move(interval_capturer);
    return result;
  }

protected:
  client::logger logger{"desktop_sharing_previews_impl"};
  boost::inline_executor executor;
  std::unique_ptr<rtc::google::capture::desktop::enumerator> enumerator =
      rtc::google::capture::desktop::enumerator::create();
  const std::shared_ptr<utils::timer_factory> timer_factory;
  const std::unique_ptr<utils::interval_timer> timer;
  previews screen_previews;
  previews window_previews;
};
#if 0
#endif
class desktop_sharing_impl final : public desktop_sharing {
public:
  desktop_sharing_impl(
      const std::shared_ptr<utils::timer_factory> timer_factory,
      const std::shared_ptr<tracks_adder> tracks_adder_,
      const std::shared_ptr<add_video_to_connection_factory>
          add_video_to_connection_factory_,
      const std::shared_ptr<video_settings> video_settings_,
      const std::shared_ptr<leaver> leaver_)
      : timer_factory{timer_factory}, tracks_adder_{tracks_adder_},
        add_video_to_connection_factory_{add_video_to_connection_factory_},
        video_settings_{video_settings_}, leaver_{leaver_} {
    video_settings_->on_paused.connect(
        [this](bool paused) { on_video_paused(paused); });
    leaver_->on_about_to_leave.connect([this] { on_leave(); });
  }

  void on_leave() {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    if (get())
      stop_or_reset();
  }

  void on_video_paused(bool paused) {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", paused:" << paused << ", did_pause:" << did_pause;
    // we did pause it, but some one else unpaused it.
    // by setting `did_pause = false` we ensure that we don't try to unpause it
    // again.
    if (did_pause && !paused)
      did_pause = false;
    // video got unpaused so let's stop desktop sharing
    if (!paused && set_capturer)
      reset();
  }

  void pause_video() {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", did_pause:" << did_pause;
    if (did_pause) // may happen on replace (two time `set` without `reset`)
      return;
    if (!video_settings_->is_a_video_available())
      return;
    if (video_settings_->get_paused())
      return;
    video_settings_->pause(true);
    did_pause = true;
  }

  void unpause_video() {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", did_pause:" << did_pause;
    if (!did_pause)
      return;
    did_pause = false;
    video_settings_->pause(false);
  }

  void set(std::intptr_t id) override {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", id:" << id;
    if (set_capturer)
      reset();
    BOOST_ASSERT(!set_capturer);
    auto capturer = rtc::google::capture::desktop::capturer::create(id);
    static const std::chrono::steady_clock::duration interval =
        std::chrono::milliseconds(1000 / 10); // 10 frames per second
    auto timer = timer_factory->create_interval_timer(interval);
    set_capturer = rtc::google::capture::desktop::interval_capturer::create(
        std::move(timer), std::move(capturer));
    auto video_source = set_capturer->get_capturer().get_source();
    pause_video();
    BOOST_ASSERT(video_source);
    video_adder = add_video_to_connection_factory_->create(video_source);
    std::dynamic_pointer_cast<rtc::google::video_track_source>(
        video_adder->get_video_track())
        ->set_content_hint(
            rtc::google::video_track_source::content_hint::detailed);
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
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
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
    auto signal_arg = get();
    decltype(set_capturer) moved_set_capturer = std::move(set_capturer);
    decltype(video_adder) moved_video_adder = std::move(video_adder);
    on_removed(signal_arg);
    tracks_adder_->remove(moved_video_adder);
  }

  void reset() override {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    stop_or_reset();
    unpause_video();
  }

protected:
  client::logger logger{"desktop_sharing_impl"};
  boost::inline_executor executor;
  std::shared_ptr<utils::timer_factory> timer_factory;
  std::shared_ptr<rtc::google::capture::desktop::interval_capturer>
      set_capturer;
  std::shared_ptr<add_video_to_connection> video_adder;
  std::shared_ptr<tracks_adder> tracks_adder_;
  std::shared_ptr<add_video_to_connection_factory>
      add_video_to_connection_factory_;
  std::shared_ptr<video_settings> video_settings_;
  std::shared_ptr<leaver> leaver_;
  bool did_pause{};
};
class desktop_sharing_noop final : public desktop_sharing {
public:
  void set([[maybe_unused]] std::intptr_t id) override {}
  std::shared_ptr<rtc::google::video_source> get() override { return nullptr; }
  void reset() override {}
};
} // namespace

std::unique_ptr<desktop_sharing_previews> desktop_sharing_previews::create(
    const std::shared_ptr<utils::timer_factory> timer_factory) {
  return std::make_unique<desktop_sharing_previews_impl>(timer_factory);
}

std::unique_ptr<desktop_sharing> desktop_sharing::create(
    const std::shared_ptr<utils::timer_factory> timer_factory,
    const std::shared_ptr<tracks_adder> tracks_adder_,
    const std::shared_ptr<add_video_to_connection_factory>
        add_video_to_connection_factory_,
    const std::shared_ptr<video_settings> video_settings_,
    const std::shared_ptr<leaver> leaver_) {
  return std::make_unique<desktop_sharing_impl>(
      timer_factory, tracks_adder_, add_video_to_connection_factory_,
      video_settings_, leaver_);
}

std::unique_ptr<desktop_sharing> desktop_sharing::create_noop() {
  return std::make_unique<desktop_sharing_noop>();
}
