#ifndef UUID_117B3F67_A885_4B3D_88AB_85BA3EC78A1B
#define UUID_117B3F67_A885_4B3D_88AB_85BA3EC78A1B

#include "client/audio_module.hpp"
#include "client/audio_settings_module.hpp"
#include "core_module.hpp"

namespace audio_client {
class audio_client {
public:
  struct config {
    bool send_audio{true};
    client::core_module::config core;
    client::audio_module::config audio;
    client::audio_settings_module::config audio_settings;
  };
  static std::unique_ptr<audio_client> create(const config &config_);
  virtual ~audio_client() = default;
  virtual int run() = 0;

protected:
};
} // namespace audio_client

#endif
