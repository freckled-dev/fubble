#ifndef UUID_30E69F1A_B38F_472F_A611_24CC8113CBF1
#define UUID_30E69F1A_B38F_472F_A611_24CC8113CBF1

#include "fubble/rtc/logger.hpp"
#include "track.hpp"

namespace webrtc {
class AudioTrackInterface;
}

namespace rtc {
namespace google {
class audio_source;
class audio_track : public track {
public:
  audio_track(const rtc::scoped_refptr<webrtc::AudioTrackInterface> &track);
  virtual audio_source &get_source() = 0;
  virtual void set_volume(double) = 0;

protected:
  rtc::logger logger{"audio_track"};
};
} // namespace google
} // namespace rtc

#endif
