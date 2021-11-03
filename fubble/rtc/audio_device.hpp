#ifndef UUID_E7F0FB1C_8C9E_4AA3_8D2C_9ADAD9A119ED
#define UUID_E7F0FB1C_8C9E_4AA3_8D2C_9ADAD9A119ED

#include <fubble/rtc/audio_source.hpp>
#include <fubble/utils/export.hpp>

namespace rtc {
class FUBBLE_PUBLIC audio_device {
public:
  virtual ~audio_device() = default;
  virtual audio_source &get_source() = 0;
};
} // namespace rtc

#endif
