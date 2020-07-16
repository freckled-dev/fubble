#ifndef RTC_GOOGLE_CAPTURE_VIDEO_ENUMERATOR_HPP
#define RTC_GOOGLE_CAPTURE_VIDEO_ENUMERATOR_HPP

#include "information.hpp"
#include "rtc/logger.hpp"

namespace rtc {
namespace google {
namespace capture {
namespace video {

// TODO refactor to devices and add signal for device_added/removed
class enumerator {
public:
  enumerator();
  virtual ~enumerator() = default;

  virtual std::vector<information> enumerate();

private:
  rtc::logger logger{"enumerator"};
};
class enumerator_noop : public enumerator {
  std::vector<information> enumerate() override { return {}; }
};

} // namespace video
} // namespace capture
} // namespace google
} // namespace rtc

#endif
