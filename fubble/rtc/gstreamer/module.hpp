#ifndef UUID_9F5F7B9C_BBDD_4550_8083_D18FDC9F8708
#define UUID_9F5F7B9C_BBDD_4550_8083_D18FDC9F8708

#include <fubble/rtc/gstreamer/settings.hpp>
#include <fubble/rtc/module.hpp>
#include <fubble/utils/executor_module.hpp>

namespace rtc::gstreamer {
class FUBBLE_PUBLIC module : public rtc::module {
public:
  module(std::shared_ptr<utils::executor_module> executor_module,
         const settings rtc_settings);
  virtual ~module();

  std::shared_ptr<rtc::factory> get_factory() override;
  std::shared_ptr<rtc::video_device_factory>
  get_video_device_creator() override;

protected:
  std::shared_ptr<utils::executor_module> executor_module;
  const settings rtc_settings;

  std::shared_ptr<rtc::factory> factory_;
  std::shared_ptr<rtc::video_device_factory> video_device_factory_;
};
} // namespace rtc::gstreamer

#endif
