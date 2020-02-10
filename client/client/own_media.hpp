#ifndef UUID_BE2C6F4A_FF34_4689_941A_8BF68E644DA1
#define UUID_BE2C6F4A_FF34_4689_941A_8BF68E644DA1

#include "rtc/google/video_source_ptr.hpp"
#include <vector>

namespace client {
class own_media {
public:
  void add_video(rtc::google::video_source &video) { videos.push_back(&video); }

  using videos_type = std::vector<rtc::google::video_source *>;
  videos_type get_videos() const { return videos; }

protected:
  videos_type videos;
};
} // namespace client

#endif
