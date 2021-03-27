#ifndef UUID_6383B542_0236_4CAC_A21D_6C73C6B5BFB9
#define UUID_6383B542_0236_4CAC_A21D_6C73C6B5BFB9

#include <fubble/app/core_module.hpp>
#include <fubble/client/audio_module.hpp>
#include <fubble/client/audio_settings_module.hpp>
#include <fubble/client/video_module.hpp>
#include <fubble/utils/export.hpp>
#include <fubble/v4l2_hw_h264/config.hpp>
#include <functional>

namespace fubble {
class FUBBLE_PUBLIC cli_client {
public:
  struct config {
    bool send_audio{true};
    bool send_video{true};
    bool use_v4l2_hw_h264{false};
    std::string room_name{"fun"};
    ::client::core_module::config core;
    ::client::audio_module::config audio;
    ::client::audio_settings_module::config audio_settings;
    ::client::video_module::config video;
    v4l2_hw_h264::config v4l2_hw_h264_config;
  };
  static std::unique_ptr<cli_client> create(const config &config_);
  virtual ~cli_client() = default;

  virtual int run() = 0;
  virtual void stop() = 0;

  virtual std::shared_ptr<::client::core_module> get_core() const = 0;
};
} // namespace fubble

#endif
