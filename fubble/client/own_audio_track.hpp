#ifndef UUID_631E04BA_F839_4AA8_B0CD_A2EE7CED7B28
#define UUID_631E04BA_F839_4AA8_B0CD_A2EE7CED7B28

#include <fubble/rtc/audio_source.hpp>
#include <fubble/rtc/audio_track.hpp>
#include <fubble/rtc/factory.hpp>
#include <memory>

namespace client {
class own_audio_track {
public:
  virtual ~own_audio_track() = default;
  virtual void set_enabled(bool) = 0;
  virtual std::shared_ptr<rtc::audio_track> get_track() = 0;

  static std::unique_ptr<own_audio_track>
  create(rtc::factory &rtc_factory, rtc::audio_source &audio_source);
  static std::unique_ptr<own_audio_track> create_noop();
};
} // namespace client

#endif
