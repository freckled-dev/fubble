#include "video_devices.hpp"
#include "rtc/logger.hpp"
#include "utils/timer.hpp"

using namespace rtc;

namespace {
class interval_video_devices : public video_devices {
public:
  interval_video_devices(video_devices &adopt, utils::interval_timer &timer)
      : delegate(adopt), timer(timer) {
    timer.start([this] { on_timeout(); });
    on_timeout();
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
};
} // namespace

std::unique_ptr<video_devices>
video_devices::create_interval_enumerating(video_devices &adopt,
                                           utils::interval_timer &timer) {
  return std::make_unique<interval_video_devices>(adopt, timer);
}
