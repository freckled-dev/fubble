#ifndef UUID_6CC52509_B0C4_43D5_92AC_618EF31ECDCF
#define UUID_6CC52509_B0C4_43D5_92AC_618EF31ECDCF

#include <boost/signals2/signal.hpp>
#include <fubble/rtc/audio_data.hpp>
#include <fubble/rtc/logger.hpp>
#include <fubble/utils/export.hpp>

namespace webrtc {
class AudioSourceInterface;
}

namespace rtc {
namespace google {
class FUBBLE_PUBLIC audio_source {
public:
  audio_source(webrtc::AudioSourceInterface &native);
  virtual ~audio_source();
  webrtc::AudioSourceInterface &get_native() const;

  boost::signals2::signal<void(const audio_data &)> on_data;

protected:
  rtc::logger logger{"audio_source"};
  webrtc::AudioSourceInterface &native_audio_track;
  struct sink;
  std::unique_ptr<sink> sink_;
};
} // namespace google
} // namespace rtc

#endif
