#ifndef UUID_BE2C6F4A_FF34_4689_941A_8BF68E644DA1
#define UUID_BE2C6F4A_FF34_4689_941A_8BF68E644DA1

#include "rtc/google/video_source_ptr.hpp"
#include <boost/assert.hpp>
#include <vector>

namespace rtc::google {
class audio_source;
}

namespace client {
class own_media {
public:
  void add_video(rtc::google::video_source &video) { videos.push_back(&video); }
  using videos_type = std::vector<rtc::google::video_source *>;
  videos_type get_videos() const { return videos; }

  void set_audio(rtc::google::audio_source &source) {
    BOOST_ASSERT(audio == nullptr);
    audio = &source;
  }
  rtc::google::audio_source *get_audio() const { return audio; }

protected:
  videos_type videos;
  rtc::google::audio_source *audio{};
};
} // namespace client

#endif
