#ifndef RTC_GOOGLE_CAPTURE_VIDEO_DEVICE_HPP
#define RTC_GOOGLE_CAPTURE_VIDEO_DEVICE_HPP

#include <fubble/rtc/logger.hpp>
#include <fubble/rtc/video_capability.hpp>
#include <fubble/rtc/video_device_factory.hpp>
#include <fubble/rtc/video_source.hpp>
#include <vector>

namespace rtc {
namespace google {
namespace capture {
namespace video {
class device : public rtc::video_device {
public:
  device() = default;
  virtual ~device() = default;
};

class device_factory : public rtc::video_device_factory {
public:
  device_factory();
  virtual ~device_factory() = default;

  std::unique_ptr<rtc::video_device> create(const std::string &id) override;

protected:
  struct device_with_start_stop_counter {
    std::shared_ptr<device> device_;
    std::shared_ptr<int> counter;
  };
  std::vector<device_with_start_stop_counter> devices;
};

} // namespace video
} // namespace capture
} // namespace google
} // namespace rtc

#endif
