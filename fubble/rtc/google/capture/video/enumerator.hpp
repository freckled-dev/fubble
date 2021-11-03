#ifndef RTC_GOOGLE_CAPTURE_VIDEO_ENUMERATOR_HPP
#define RTC_GOOGLE_CAPTURE_VIDEO_ENUMERATOR_HPP

#include <fubble/rtc/logger.hpp>
#include <fubble/rtc/video_devices.hpp>
#include <fubble/utils/export.hpp>

namespace rtc {
namespace google {
namespace capture {
namespace video {

using information = rtc::video_devices::information;
// TODO refactor to devices and add signal for device_added/removed
// TODO rename to `video_devices`
class FUBBLE_PUBLIC enumerator : public rtc::video_devices {
public:
  enumerator();
  ~enumerator() = default;

  bool enumerate() override;
  std::vector<information> get_enumerated() const override;

private:
  rtc::logger logger{"enumerator"};
  std::vector<information> enumerated;
};

} // namespace video
} // namespace capture
} // namespace google
} // namespace rtc

#endif
