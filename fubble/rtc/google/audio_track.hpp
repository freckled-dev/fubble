#ifndef UUID_30E69F1A_B38F_472F_A611_24CC8113CBF1
#define UUID_30E69F1A_B38F_472F_A611_24CC8113CBF1

#include <fubble/rtc/audio_track.hpp>
#include <fubble/rtc/google/track.hpp>

namespace rtc {
namespace google {
class audio_track : public rtc::google::track, public rtc::audio_track {
public:
};
} // namespace google
} // namespace rtc

#endif
