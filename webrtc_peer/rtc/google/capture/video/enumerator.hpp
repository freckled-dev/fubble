#ifndef RTC_GOOGLE_CAPTURE_VIDEO_ENUMERATOR_HPP
#define RTC_GOOGLE_CAPTURE_VIDEO_ENUMERATOR_HPP

#include "information.hpp"
#include "rtc/logger.hpp"

namespace rtc {
namespace google {
namespace capture {
namespace video {

class enumerator {
public:
  enumerator();

  std::vector<information> operator()();

private:
  class logger logger {
    "enumerator"
  };
};

} // namespace video
} // namespace capture
} // namespace google
} // namespace rtc

#endif
