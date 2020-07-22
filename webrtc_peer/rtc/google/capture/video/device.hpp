#ifndef RTC_GOOGLE_CAPTURE_VIDEO_DEVICE_HPP
#define RTC_GOOGLE_CAPTURE_VIDEO_DEVICE_HPP

#include "rtc/google/video_source.hpp"
#include "rtc/logger.hpp"

namespace rtc {
namespace google {
namespace capture {
namespace video {
class device : public video_source {
public:
  device() = default;
  virtual ~device() = default;

  virtual void start() = 0;
  virtual void stop() = 0;

  virtual std::string get_id() const = 0;
};
class device_factory {
public:
  device_factory();

  std::unique_ptr<device> create(const std::string &id);
};
} // namespace video
} // namespace capture
} // namespace google
} // namespace rtc

#endif
