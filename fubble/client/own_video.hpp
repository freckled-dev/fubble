#ifndef UUID_579F5792_D570_4034_9384_6879608D8624
#define UUID_579F5792_D570_4034_9384_6879608D8624

#include <fubble/utils/signal.hpp>
#include <fubble/rtc/video_source.hpp>
#include <vector>

namespace client {
// TODO refactor to `own_videos`, plural
class own_video {
public:
  virtual ~own_video() = default;
  virtual void add(std::shared_ptr<rtc::video_source> video) = 0;
  virtual void remove(std::shared_ptr<rtc::video_source> video) = 0;

  using videos_type = std::vector<std::shared_ptr<rtc::video_source>>;
  virtual videos_type get_all() const = 0;

  utils::signal::signal<std::shared_ptr<rtc::video_source>> on_added;
  utils::signal::signal<std::shared_ptr<rtc::video_source>> on_removed;

  static std::unique_ptr<own_video> create();
};
} // namespace client

#endif
