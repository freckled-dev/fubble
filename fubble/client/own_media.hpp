#ifndef UUID_BE2C6F4A_FF34_4689_941A_8BF68E644DA1
#define UUID_BE2C6F4A_FF34_4689_941A_8BF68E644DA1

#include <boost/assert.hpp>
#include <boost/signals2/signal.hpp>
#include <fubble/client/own_video.hpp>
#include <fubble/rtc/audio_track.hpp>
#include <fubble/rtc/video_source.hpp>

namespace client {
class own_audio_track;
class desktop_sharing;
class own_media {
public:
  own_media();
  virtual ~own_media();

  // TODO refactor #355
  void
  set_own_audio_track(const std::shared_ptr<own_audio_track> &own_audio_track_);
  void set_own_video(const std::shared_ptr<own_video> &own_video_);
  void set_desktop_sharing(std::shared_ptr<desktop_sharing> desktop_sharing_);

  rtc::audio_track *get_audio() const;
  // may return nullptr!
  std::shared_ptr<own_video> get_videos() const;
  // may return nullptr!
  std::shared_ptr<desktop_sharing> get_desktop_sharing() const;

protected:
  std::shared_ptr<own_audio_track> own_audio_track_;
  std::shared_ptr<own_video> own_video_;
  std::shared_ptr<desktop_sharing> desktop_sharing_;
};
} // namespace client

#endif
