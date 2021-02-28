#ifndef UUID_C58352CC_8B1C_4D14_B3D3_FCF195857FB6
#define UUID_C58352CC_8B1C_4D14_B3D3_FCF195857FB6

#include <fubble/rtc/google/settings.hpp>
#include <fubble/rtc/module.hpp>
#include <fubble/utils/executor_module.hpp>

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
class asio_signaling_thread;
class factory;
class video_encoder_factory_factory;

class FUBBLE_PUBLIC module : public rtc::module {
public:
  module(std::shared_ptr<utils::executor_module> executor_module,
         const settings rtc_settings);
  virtual ~module();

  std::shared_ptr<asio_signaling_thread> get_asio_signaling_thread();
  std::shared_ptr<rtc::factory> get_factory() override;
  std::shared_ptr<capture::audio::device_creator> get_audio_device_creator();
  std::shared_ptr<capture::audio::device> get_audio_device();
  std::shared_ptr<rtc::video_device_factory>
  get_video_device_creator() override;
  virtual std::shared_ptr<rtc::google::video_encoder_factory_factory>
  get_video_encoder_factory_factory();

protected:
  std::shared_ptr<utils::executor_module> executor_module;
  const settings rtc_settings;
  std::shared_ptr<asio_signaling_thread> asio_signaling_thread_;
  std::shared_ptr<factory> factory_;
  std::shared_ptr<capture::audio::device_creator> audio_device_creator;
  std::shared_ptr<capture::audio::device> audio_device;
  std::shared_ptr<rtc::video_device_factory> video_device_creator;
  std::shared_ptr<rtc::google::video_encoder_factory_factory>
      video_encoder_factory_factory_;
};
} // namespace google
} // namespace rtc

#endif
