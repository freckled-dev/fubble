#ifndef UUID_6CC52509_B0C4_43D5_92AC_618EF31ECDCF
#define UUID_6CC52509_B0C4_43D5_92AC_618EF31ECDCF

#include <api/media_stream_interface.h>

namespace rtc::google {
class audio_source {
public:
  virtual ~audio_source() = default;
  virtual webrtc::AudioSourceInterface &get_native() const = 0;
};
} // namespace rtc::google

#endif
