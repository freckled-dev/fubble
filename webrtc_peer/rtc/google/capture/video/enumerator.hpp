#ifndef RTC_GOOGLE_CAPTURE_VIDEO_ENUMERATOR_HPP
#define RTC_GOOGLE_CAPTURE_VIDEO_ENUMERATOR_HPP

#include "information.hpp"
#include <logging/logger.hpp>

namespace rtc::google::capture::video {

class enumerator {
public:
  enumerator();

  std::vector<information> operator()();

private:
  logging::logger logger;
};

} // namespace rtc::google::capture::video

#endif

