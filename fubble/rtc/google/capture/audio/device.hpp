#ifndef UUID_A6DF372E_9854_4798_95D4_28504F6A0DF5
#define UUID_A6DF372E_9854_4798_95D4_28504F6A0DF5

// TODO move to an impl file
#include <api/media_stream_interface.h>
#include <fubble/utils/export.hpp>

namespace rtc {
namespace google {
class audio_source;
namespace capture {
namespace audio {
class FUBBLE_PUBLIC device {
public:
  using native_ptr = rtc::scoped_refptr<webrtc::AudioSourceInterface>;
  device(const native_ptr &native);
  ~device();

  audio_source &get_source();

protected:
  native_ptr native;
  std::unique_ptr<audio_source> source;
};
} // namespace audio
} // namespace capture
} // namespace google
} // namespace rtc

#endif
