#include "video_devices.hpp"
#include "fubble/utils/timer.hpp"
#include "rtc/logger.hpp"

using namespace rtc;

namespace {
class interval_video_devices : public video_devices {
public:
  interval_video_devices(video_devices &adopt, utils::interval_timer &timer)
      : delegate(adopt), timer(timer) {
    on_timeout();
    timer.start([this] { on_timeout(); });
    enumerated_changed_connection = delegate.on_enumerated_changed.connect(
        [this] { on_enumerated_changed(); });
  }

  ~interval_video_devices() { timer.stop(); }

  void on_timeout() { enumerate(); }

  bool enumerate() override { return delegate.enumerate(); }

  std::vector<information> get_enumerated() const override {
    return delegate.get_enumerated();
  }

protected:
  rtc::logger logger{"interval_video_devices"};
  video_devices &delegate;
  utils::interval_timer &timer;
  boost::signals2::scoped_connection enumerated_changed_connection;
};
} // namespace

std::unique_ptr<video_devices>
video_devices::create_interval_enumerating(video_devices &adopt,
                                           utils::interval_timer &timer) {
  return std::make_unique<interval_video_devices>(adopt, timer);
}
