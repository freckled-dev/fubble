#ifndef UUID_BE2C6F4A_FF34_4689_941A_8BF68E644DA1
#define UUID_BE2C6F4A_FF34_4689_941A_8BF68E644DA1

#include "rtc/google/video_source_ptr.hpp"
#include <boost/assert.hpp>
#include <boost/signals2/signal.hpp>
#include <optional>
#include <vector>

namespace rtc::google {
class audio_track;
}

namespace client {
class own_audio_track;
// TODO `video_settings` and `own_media` are doing stuff that belongs to a new
// class, eg `own_videos`. `video_setting` is doing device and tracks handling
// currently this class is getting used for own_participant videos list.
class own_media {
public:
  own_media(own_audio_track &own_audio_track_);
  virtual ~own_media() = default;

  // TODO create and move to own_video
  virtual void add_video(rtc::google::video_source &video);
  virtual void remove_video(rtc::google::video_source &video);

  boost::signals2::signal<void(rtc::google::video_source &)> on_video_added;
  boost::signals2::signal<void(rtc::google::video_source &)> on_video_removed;

  using videos_type = std::vector<rtc::google::video_source *>;
  videos_type get_videos() const { return videos; }

  rtc::google::audio_track *get_audio() const;

protected:
  videos_type videos;
  own_audio_track &own_audio_track_;
};
} // namespace client

#endif
