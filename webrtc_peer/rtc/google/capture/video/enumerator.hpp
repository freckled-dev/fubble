#ifndef RTC_GOOGLE_CAPTURE_VIDEO_ENUMERATOR_HPP
#define RTC_GOOGLE_CAPTURE_VIDEO_ENUMERATOR_HPP

#include "information.hpp"
#include "rtc/logger.hpp"

namespace rtc::google::capture::video {

class enumerator {
public:
  enumerator();

  std::vector<information> operator()();

private:
  class logger logger {
    "enumerator"
  };
};

} // namespace rtc::google::capture::video

#endif
