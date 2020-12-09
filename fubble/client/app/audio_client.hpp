#ifndef UUID_117B3F67_A885_4B3D_88AB_85BA3EC78A1B
#define UUID_117B3F67_A885_4B3D_88AB_85BA3EC78A1B

#include "client/audio_module.hpp"
#include "client/audio_settings_module.hpp"
#include "core_module.hpp"
#include "fubble/utils/export.hpp"
#include <boost/thread/future.hpp>

namespace audio_client {
class FUBBLE_PUBLIC audio_client {
public:
  struct config {
    bool send_audio{true};
    client::core_module::config core;
    client::audio_module::config audio;
    client::audio_settings_module::config audio_settings;
  };
  static std::unique_ptr<audio_client> create(const config &config_);
  virtual ~audio_client() = default;

  // TODO actually this method will block! change from future to `optional`,
  // `outcome` or `expected`
  virtual boost::future<int> run() = 0;
  virtual void stop() = 0;

  virtual std::shared_ptr<client::core_module> get_core() const = 0;

protected:
};
} // namespace audio_client

#endif
