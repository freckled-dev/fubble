#ifndef UUID_8E758833_BF07_4220_AA70_519F1DDEE760
#define UUID_8E758833_BF07_4220_AA70_519F1DDEE760

#include <fubble/rtc/google/capture/video/device.hpp>
#include <fubble/rtc/google/capture/video/enumerator.hpp>

namespace video_capture_qt {

class enumerator {
public:
};

class device : public rtc::google::capture::video::device {
public:
  static std::unique_ptr<rtc::google::capture::video::device> create();
};

} // namespace video_capture_qt

#endif
