#ifndef UUID_3F71B506_929B_4811_AFB6_2C552E301C13
#define UUID_3F71B506_929B_4811_AFB6_2C552E301C13

#include <fubble/rtc/video_device.hpp>
#include <memory>
#include <string>

namespace rtc {
class video_device_factory {
public:
  virtual ~video_device_factory() = default;

  virtual std::unique_ptr<video_device> create(const std::string &id) = 0;
};
} // namespace rtc

#endif
