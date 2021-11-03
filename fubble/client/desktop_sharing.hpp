#ifndef UUID_F7A69AED_205D_4411_BD0C_BDC8A16E3D4D
#define UUID_F7A69AED_205D_4411_BD0C_BDC8A16E3D4D

#include <boost/signals2/signal.hpp>
#include <fubble/client/add_video_to_connection.hpp>
#include <fubble/client/leaver.hpp>
#include <fubble/client/tracks_adder.hpp>
#include <fubble/client/video_settings.hpp>
#include <fubble/rtc/google/capture/desktop/capturer.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace utils {
class timer_factory;
}

namespace client {
class desktop_sharing_previews {
public:
  virtual ~desktop_sharing_previews() = default;

  struct preview {
    std::shared_ptr<rtc::google::capture::desktop::interval_capturer> capturer;
  };
  using previews = std::vector<preview>;
#if 0
  virtual previews get_all() = 0;
#endif
  virtual previews get_screens() = 0;
  boost::signals2::signal<void(preview)> on_screen_added;
  boost::signals2::signal<void(preview)> on_screen_removed;
  virtual previews get_windows() = 0;
  boost::signals2::signal<void(preview)> on_window_added;
  boost::signals2::signal<void(preview)> on_window_removed;

  virtual void start() = 0;
  virtual void stop() = 0;

  static std::unique_ptr<desktop_sharing_previews>
  create(const std::shared_ptr<utils::timer_factory> timer_factory);
};

class desktop_sharing {
public:
  virtual ~desktop_sharing() = default;
  enum class type { screen, window };
  virtual void set(std::intptr_t id) = 0;
  using video_ptr = std::shared_ptr<rtc::video_source>;
  virtual video_ptr get() = 0;
  virtual void reset() = 0;

  boost::signals2::signal<void(video_ptr)> on_added;
  boost::signals2::signal<void(video_ptr)> on_removed;

  static std::unique_ptr<desktop_sharing>
  create(const std::shared_ptr<utils::timer_factory> timer_factory,
         const std::shared_ptr<tracks_adder> tracks_adder_,
         const std::shared_ptr<add_video_to_connection_factory>
             add_video_to_connection_factory_,
         const std::shared_ptr<video_settings> video_settings_,
         const std::shared_ptr<leaver> leaver_);
  static std::unique_ptr<desktop_sharing> create_noop();
};
} // namespace client

#endif
