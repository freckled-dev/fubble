#ifndef UUID_56991B82_F65A_4091_B353_801DDE773A7E
#define UUID_56991B82_F65A_4091_B353_801DDE773A7E

#include <chrono>
#include <fubble/client/session_module.hpp>
#include <fubble/rtc/module.hpp>
#include <fubble/utils/executor_module.hpp>

namespace utils {
class interval_timer;
}
namespace rtc {
class video_devices;
}
namespace client {
class add_video_to_connection_factory;
class own_video;
class video_settings;
class FUBBLE_PUBLIC video_module {
public:
  struct config {
    bool enabled{true};
    std::chrono::steady_clock::duration video_enumerator_refresh_timeout =
        std::chrono::seconds(1);
    rtc::video::capability capability;
  };
  video_module(std::shared_ptr<utils::executor_module> executor_module,
               std::shared_ptr<rtc::module> rtc_module,
               std::shared_ptr<session_module> session_module_,
               const config &config_);

  std::shared_ptr<rtc::video_devices> get_enumerator();
  std::shared_ptr<add_video_to_connection_factory>
  get_add_video_to_connection_factory();
  std::shared_ptr<own_video> get_own_video();
  std::shared_ptr<video_settings> get_video_settings();

protected:
  std::shared_ptr<utils::executor_module> executor_module;
  std::shared_ptr<rtc::module> rtc_module;
  std::shared_ptr<session_module> session_module_;
  const config config_;

  std::shared_ptr<utils::interval_timer> video_enumerator_timer;
  std::shared_ptr<rtc::video_devices> video_enumerator_google;
  std::shared_ptr<rtc::video_devices> video_enumerator;
  std::shared_ptr<add_video_to_connection_factory>
      add_video_to_connection_factory_;
  std::shared_ptr<own_video> own_video_;
  std::shared_ptr<video_settings> video_settings_;
};
} // namespace client

#endif
