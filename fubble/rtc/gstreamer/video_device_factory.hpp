#ifndef UUID_DC2DA26C_4CD5_425F_AC47_F081E5344535
#define UUID_DC2DA26C_4CD5_425F_AC47_F081E5344535

#include <fubble/rtc/video_device_factory.hpp>

namespace rtc::gstreamer {
class video_device_factory : public rtc::video_device_factory {
public:
  ~video_device_factory();
  std::unique_ptr<rtc::video_device> create(const std::string &id) override;
};
} // namespace rtc::gstreamer

#endif
