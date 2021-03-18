#ifndef UUID_35C0E76A_8CA4_4636_8CAA_FF1FB40B1B74
#define UUID_35C0E76A_8CA4_4636_8CAA_FF1FB40B1B74

#include <fubble/rtc/factory.hpp>
#include <fubble/rtc/video_device_factory.hpp>
#include <fubble/utils/export.hpp>

namespace rtc {
class FUBBLE_PUBLIC module {
public:
  virtual ~module() = default;
  virtual std::shared_ptr<factory> get_factory() = 0;
  virtual std::shared_ptr<video_device_factory> get_video_device_creator() = 0;
};
} // namespace rtc

#endif
