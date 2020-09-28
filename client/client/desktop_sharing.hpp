#ifndef UUID_F7A69AED_205D_4411_BD0C_BDC8A16E3D4D
#define UUID_F7A69AED_205D_4411_BD0C_BDC8A16E3D4D

#include "client/add_video_to_connection.hpp"
#include "client/tracks_adder.hpp"
#include "rtc/google/capture/desktop/capturer.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace rtc::google::capture::desktop {
class interval_capturer;
}
namespace utils {
class timer_factory;
}

namespace client {
class desktop_sharing {
public:
  virtual ~desktop_sharing() = default;
  enum class type { screen, window };
  virtual void set(std::intptr_t id) = 0;
  virtual void reset() = 0;

  struct preview {
    std::unique_ptr<rtc::google::capture::desktop::interval_capturer> capturer;
  };
  using previews = std::vector<preview>;
  virtual previews get_screen_previews() = 0;
  virtual previews get_window_previews() = 0;

  static std::unique_ptr<desktop_sharing>
  create(const std::shared_ptr<utils::timer_factory> timer_factory,
         const std::shared_ptr<tracks_adder> tracks_adder_,
         const std::shared_ptr<add_video_to_connection_factory>
             add_video_to_connection_factory_);
  static std::unique_ptr<desktop_sharing> create_noop();
};
} // namespace client

#endif
