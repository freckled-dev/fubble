#ifndef UUID_DE3E4714_AD73_44A4_9ED2_AA1F55903106
#define UUID_DE3E4714_AD73_44A4_9ED2_AA1F55903106

#include "client/logger.hpp"
#include <memory>

namespace rtc::google {
class video_source;
namespace capture::video {
class enumerator;
class device_creator;
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
      rtc::google::capture::video::device_creator &device_creator,
      own_media &own_media_, tracks_adder &tracks_adder_,
      add_video_to_connection_factory &add_video_to_connection_factory_);
  ~video_settings();

  rtc::google::video_source *get_video_source() const;

protected:
  client::logger logger{"video_settings"};
  rtc::google::capture::video::enumerator &enumerator;
  rtc::google::capture::video::device_creator &device_creator;
  own_media &own_media_;
  tracks_adder &tracks_adder_;
  add_video_to_connection_factory &add_video_to_connection_factory_;

  std::shared_ptr<rtc::google::capture::video::device> capture_device;
  std::unique_ptr<add_video_to_connection> video_track_adder;
};
} // namespace client

#endif
