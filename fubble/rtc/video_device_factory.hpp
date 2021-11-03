#ifndef UUID_3F71B506_929B_4811_AFB6_2C552E301C13
#define UUID_3F71B506_929B_4811_AFB6_2C552E301C13

#include <fubble/rtc/video_device.hpp>
#include <fubble/utils/export.hpp>
#include <memory>
#include <string>

namespace rtc {
class FUBBLE_PUBLIC video_device_factory {
public:
  virtual ~video_device_factory() = default;

  virtual std::unique_ptr<video_device> create(const std::string &id) = 0;
};

class video_device_factory_noop : public video_device_factory {
public:
  std::unique_ptr<video_device> create(const std::string &id) override;
};
} // namespace rtc

#endif
