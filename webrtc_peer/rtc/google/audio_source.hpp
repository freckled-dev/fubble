#ifndef UUID_6CC52509_B0C4_43D5_92AC_618EF31ECDCF
#define UUID_6CC52509_B0C4_43D5_92AC_618EF31ECDCF

#include "audio_data.hpp"
#include "rtc/logger.hpp"
#include <api/media_stream_interface.h>
#if 0
#include <boost/signals2/signal.hpp>
#endif

namespace rtc {
namespace google {
class audio_source
#if 0
  : public webrtc::AudioTrackSinkInterface
#endif
{
public:
  audio_source(webrtc::AudioSourceInterface &native);
  virtual ~audio_source();
  webrtc::AudioSourceInterface &get_native() const;

#if 0
  boost::signals2::signal<void(const audio_data &)> on_data;
#endif

protected:
#if 0
  void OnData(const void *audio_data, int bits_per_sample, int sample_rate,
              size_t number_of_channels, size_t number_of_frames) override;
#endif

  class logger logger {
    "audio_source"
  };
  webrtc::AudioSourceInterface &native_audio_track;
};
} // namespace google
} // namespace rtc

#endif
