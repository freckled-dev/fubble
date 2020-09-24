#include "desktop_sharing.hpp"
#include "rtc/google/capture/desktop/enumerator.hpp"
#include "utils/timer.hpp"
#include <chrono>

using namespace client;

namespace {
class desktop_sharing_impl final : public desktop_sharing {
public:
  desktop_sharing_impl(std::shared_ptr<utils::timer_factory> timer_factory)
      : timer_factory{timer_factory} {}

  void set(std::string id, type type_) {
    // TODO
    (void)id;
    (void)type_;
  }

  void reset() {}

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
  std::shared_ptr<utils::timer_factory> timer_factory;
  std::unique_ptr<rtc::google::capture::desktop::enumerator> enumerator =
      rtc::google::capture::desktop::enumerator::create();
};
} // namespace

std::unique_ptr<desktop_sharing>
desktop_sharing::create(std::shared_ptr<utils::timer_factory> timer_factory) {
  return std::make_unique<desktop_sharing_impl>(timer_factory);
}
