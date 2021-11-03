#ifndef UUID_7415971C_4064_4B5C_9C36_3675417BF982
#define UUID_7415971C_4064_4B5C_9C36_3675417BF982

#include <fubble/rtc/audio_source.hpp>
#include <fubble/rtc/track.hpp>

namespace rtc {
class audio_track : public rtc::track {
public:
  virtual audio_source &get_source() = 0;
  virtual void set_volume(double) = 0;

protected:
};
} // namespace rtc

#endif
