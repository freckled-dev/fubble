#ifndef UUID_30E69F1A_B38F_472F_A611_24CC8113CBF1
#define UUID_30E69F1A_B38F_472F_A611_24CC8113CBF1

#include "rtc/logger.hpp"
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
  virtual webrtc::AudioTrackInterface &get_native_audio_track() = 0;
  virtual audio_source &get_source() = 0;

protected:
  rtc::logger logger{"audio_track"};
};
} // namespace google
} // namespace rtc

#endif
