#ifndef UUID_C58352CC_8B1C_4D14_B3D3_FCF195857FB6
#define UUID_C58352CC_8B1C_4D14_B3D3_FCF195857FB6

#include "rtc/google/settings.hpp"
#include "utils/executor_module.hpp"

namespace rtc {
namespace google {
namespace capture {
namespace audio {
class device_creator;
class device;
} // namespace audio
namespace video {
class device_factory;
}
} // namespace capture
struct settings;
class asio_signaling_thread;
class factory;
class FUBBLE_PUBLIC module {
public:
  module(std::shared_ptr<utils::executor_module> executor_module,
         const settings rtc_settings);
  virtual ~module();

  std::shared_ptr<asio_signaling_thread> get_asio_signaling_thread();
  std::shared_ptr<factory> get_factory();
  std::shared_ptr<capture::audio::device_creator> get_audio_device_creator();
  std::shared_ptr<capture::audio::device> get_audio_device();
  std::shared_ptr<capture::video::device_factory> get_video_device_creator();

protected:
  std::shared_ptr<utils::executor_module> executor_module;
  const settings rtc_settings;
  std::shared_ptr<asio_signaling_thread> asio_signaling_thread_;
  std::shared_ptr<factory> factory_;
  std::shared_ptr<capture::audio::device_creator> audio_device_creator;
  std::shared_ptr<capture::audio::device> audio_device;
  std::shared_ptr<capture::video::device_factory> video_device_creator;
};
} // namespace google
} // namespace rtc

#endif
