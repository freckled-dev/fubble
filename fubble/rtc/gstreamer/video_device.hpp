#ifndef UUID_F0EBA177_5696_4AC3_A921_BEF358991EAE
#define UUID_F0EBA177_5696_4AC3_A921_BEF358991EAE

#include <fubble/rtc/video_device.hpp>

namespace rtc::gstreamer {
class video_device : public rtc::video_device {
public:
  ~video_device();
  void start(const rtc::video::capability &cap) override;
  void stop() override;
  bool get_started() const override;
  std::shared_ptr<rtc::video_source> get_source() const override;
  std::string get_id() const override;
};
} // namespace rtc::gstreamer

#endif
