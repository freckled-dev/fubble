#ifndef UUID_30E69F1A_B38F_472F_A611_24CC8113CBF1
#define UUID_30E69F1A_B38F_472F_A611_24CC8113CBF1

#include "rtc/logger.hpp"
#include "track.hpp"
#include <boost/optional.hpp> // no c++17 for webrtc

namespace webrtc {
class AudioTrackInterface;
}

namespace rtc {
namespace google {
class audio_track : public track {
public:
  struct stats {
    boost::optional<int> rms_dbfs;
    bool voice_detected{};
  };

  virtual webrtc::AudioTrackInterface &get_native_audio_track() = 0;
  virtual stats get_stats();

protected:
  rtc::logger logger{"audio_track"};
};
} // namespace google
} // namespace rtc

#endif
