#ifndef RTC_GOOGLE_CAPTURE_VIDEO_DEVICE_CREATOR_HPP
#define RTC_GOOGLE_CAPTURE_VIDEO_DEVICE_CREATOR_HPP

#include "information.hpp"
#include <memory>

namespace rtc {
namespace google {
namespace capture {
namespace video {
class device;
class device_creator {
public:
  device_creator();

  std::unique_ptr<device> operator()(const std::string &id);
};
} // namespace video
} // namespace capture
} // namespace google
} // namespace rtc

#endif
