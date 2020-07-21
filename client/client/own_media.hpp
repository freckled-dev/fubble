#ifndef UUID_BE2C6F4A_FF34_4689_941A_8BF68E644DA1
#define UUID_BE2C6F4A_FF34_4689_941A_8BF68E644DA1

#include "rtc/google/video_source_ptr.hpp"
#include <boost/assert.hpp>
#include <boost/signals2/signal.hpp>
#include <optional>
#include <vector>

namespace rtc::google {
class audio_source;
}

namespace client {
class loopback_audio;
class own_media {
public:
  own_media(loopback_audio &own_audio_);

  // TODO create and move to own_video
  void add_video(rtc::google::video_source &video);
  void remove_video(rtc::google::video_source &video);

  boost::signals2::signal<void(rtc::google::video_source &)> on_video_added;
  boost::signals2::signal<void(rtc::google::video_source &)> on_video_removed;

  using videos_type = std::vector<rtc::google::video_source *>;
  videos_type get_videos() const { return videos; }

  rtc::google::audio_source *get_audio() const;
  // TODO rename
  client::loopback_audio &get_own_audio() const;

protected:
  videos_type videos;
  loopback_audio &own_audio_;
};
} // namespace client

#endif
