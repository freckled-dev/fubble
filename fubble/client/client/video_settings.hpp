#ifndef UUID_DE3E4714_AD73_44A4_9ED2_AA1F55903106
#define UUID_DE3E4714_AD73_44A4_9ED2_AA1F55903106

#include "client/logger.hpp"
#include "client/own_video.hpp"
#include "fubble/rtc/video_devices.hpp"
#include <boost/signals2/signal.hpp>
#include <memory>
#include <boost/optional.hpp>

namespace rtc::google {
class video_source;
namespace capture::video {
class device_factory;
class device;
} // namespace capture::video
} // namespace rtc::google

namespace client {
class add_video_to_connection;
class add_video_to_connection_factory;
class own_media;
class tracks_adder;
class video_settings {
public:
  video_settings(
      rtc::video_devices &enumerator,
      rtc::google::capture::video::device_factory &device_creator,
      own_video &own_media_, tracks_adder &tracks_adder_,
      add_video_to_connection_factory &add_video_to_connection_factory_);
  ~video_settings();

  void pause(bool paused);
  bool get_paused() const;
  boost::signals2::signal<void(bool)> on_paused;

  void change_to_device(const std::string &id);
  // means a video is instanced!
  bool is_a_video_available() const;
  boost::optional<std::string> get_device_id() const;

  std::shared_ptr<rtc::google::video_source> get_video_source() const;
  boost::signals2::signal<void()> on_video_source_changed;

protected:
  void reset_current_video_capture();
  void reset_current_video();
  void on_video_devices_changed();

  client::logger logger{"video_settings"};
  rtc::video_devices &enumerator;
  rtc::google::capture::video::device_factory &device_creator;
  own_video &own_media_;
  tracks_adder &tracks_adder_;
  add_video_to_connection_factory &add_video_to_connection_factory_;

  std::shared_ptr<rtc::google::capture::video::device> capture_device;
  std::shared_ptr<add_video_to_connection> video_track_adder;
  bool paused{};
  // TODO refactor to `current_device_id`
  boost::optional<std::string> last_device_id;
};
} // namespace client

#endif
