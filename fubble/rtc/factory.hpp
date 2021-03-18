#ifndef UUID_D663E552_1B26_418C_935A_8CF3755EDDE3
#define UUID_D663E552_1B26_418C_935A_8CF3755EDDE3

#include <fubble/rtc/audio_devices.hpp>
#include <fubble/rtc/connection.hpp>
#include <fubble/rtc/video_source.hpp>
#include <fubble/rtc/video_track.hpp>
#include <memory>

namespace rtc {

class factory {
public:
  virtual ~factory() = default;
  virtual std::unique_ptr<connection> create_connection() = 0;
  virtual std::unique_ptr<video_track>
  create_video_track(const std::shared_ptr<video_source> &source) = 0;
  virtual std::shared_ptr<audio_devices> get_audio_devices() = 0;
};
} // namespace rtc

#endif
