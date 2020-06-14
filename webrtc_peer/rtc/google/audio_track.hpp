#ifndef UUID_30E69F1A_B38F_472F_A611_24CC8113CBF1
#define UUID_30E69F1A_B38F_472F_A611_24CC8113CBF1

#include "track.hpp"

namespace webrtc {
class AudioTrackInterface;
}

namespace rtc {
namespace google {
class audio_track : public track {
public:
  virtual webrtc::AudioTrackInterface &get_native_audio_track() = 0;
};
} // namespace google
} // namespace rtc

#endif
