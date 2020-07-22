#ifndef UUID_DE3E4714_AD73_44A4_9ED2_AA1F55903106
#define UUID_DE3E4714_AD73_44A4_9ED2_AA1F55903106

#include "client/logger.hpp"
#include <boost/signals2/signal.hpp>
#include <memory>
#include <optional>

namespace rtc::google {
class video_source;
namespace capture::video {
class enumerator;
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
      rtc::google::capture::video::enumerator &enumerator,
      rtc::google::capture::video::device_factory &device_creator,
      own_media &own_media_, tracks_adder &tracks_adder_,
      add_video_to_connection_factory &add_video_to_connection_factory_);
  ~video_settings();

  void pause(bool paused);
  bool get_paused() const;
  void change_to_device(const std::string &id);
  bool is_a_video_available() const;

  rtc::google::video_source *get_video_source() const;
  boost::signals2::signal<void()> on_video_source_changed;

protected:
  void reset_current_video();

  client::logger logger{"video_settings"};
  rtc::google::capture::video::enumerator &enumerator;
  rtc::google::capture::video::device_factory &device_creator;
  own_media &own_media_;
  tracks_adder &tracks_adder_;
  add_video_to_connection_factory &add_video_to_connection_factory_;

  std::shared_ptr<rtc::google::capture::video::device> capture_device;
  std::unique_ptr<add_video_to_connection> video_track_adder;
  bool paused{};
  std::optional<std::string> last_device_id;
};
} // namespace client

#endif
