#ifndef UUID_A6DF372E_9854_4798_95D4_28504F6A0DF5
#define UUID_A6DF372E_9854_4798_95D4_28504F6A0DF5

#include "rtc/google/audio_source.hpp"

namespace rtc {
namespace google {
namespace capture {
namespace audio {
class device : public audio_source {
public:
  using native_ptr = rtc::scoped_refptr<webrtc::AudioSourceInterface>;
  device(const native_ptr &native);

protected:
  native_ptr native;
};
} // namespace audio
} // namespace capture
} // namespace google
} // namespace rtc

#endif
