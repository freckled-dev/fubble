#ifndef UUID_631E04BA_F839_4AA8_B0CD_A2EE7CED7B28
#define UUID_631E04BA_F839_4AA8_B0CD_A2EE7CED7B28

#include <memory>

namespace rtc::google {
class audio_track;
class audio_source;
class factory;
} // namespace rtc::google

namespace client {
class own_audio_track {
public:
  virtual ~own_audio_track() = default;
  virtual std::shared_ptr<rtc::google::audio_track> get_track() = 0;

  static std::unique_ptr<own_audio_track>
  create(rtc::google::factory &rtc_factory,
         rtc::google::audio_source &audio_source);
  static std::unique_ptr<own_audio_track> create_noop();
};
} // namespace client

#endif
