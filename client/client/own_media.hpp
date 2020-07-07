#ifndef UUID_BE2C6F4A_FF34_4689_941A_8BF68E644DA1
#define UUID_BE2C6F4A_FF34_4689_941A_8BF68E644DA1

#include "rtc/google/video_source_ptr.hpp"
#include <boost/assert.hpp>
#include <optional>
#include <vector>

namespace rtc::google {
class audio_source;
}

namespace client {
class own_audio;
class own_media {
public:
  own_media(own_audio &own_audio_);

  void add_video(rtc::google::video_source &video) { videos.push_back(&video); }
  using videos_type = std::vector<rtc::google::video_source *>;
  videos_type get_videos() const { return videos; }

  rtc::google::audio_source *get_audio() const;

protected:
  videos_type videos;
  own_audio &own_audio_;
};
} // namespace client

#endif
