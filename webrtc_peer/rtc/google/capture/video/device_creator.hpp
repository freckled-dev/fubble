#ifndef RTC_GOOGLE_CAPTURE_VIDEO_DEVICE_CREATOR_HPP
#define RTC_GOOGLE_CAPTURE_VIDEO_DEVICE_CREATOR_HPP

#include "information.hpp"
#include <memory>

namespace rtc::google::capture::video {
class device;
class device_creator {
public:
  device_creator();

  std::unique_ptr<device> operator()(const std::string &id);
};
} // namespace rtc::google::capture::video

#endif
