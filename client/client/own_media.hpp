#ifndef UUID_BE2C6F4A_FF34_4689_941A_8BF68E644DA1
#define UUID_BE2C6F4A_FF34_4689_941A_8BF68E644DA1

#include "client/own_video.hpp"
#include "rtc/google/video_source_ptr.hpp"
#include <boost/assert.hpp>
#include <boost/signals2/signal.hpp>

namespace rtc::google {
class audio_track;
}

namespace client {
class own_audio_track;
class own_media {
public:
  own_media(own_audio_track &own_audio_track_, own_video &own_video_);
  virtual ~own_media() = default;

  rtc::google::audio_track *get_audio() const;
  own_video &get_videos() const;

protected:
  own_video &videos;
  own_audio_track &own_audio_track_;
};
} // namespace client

#endif
