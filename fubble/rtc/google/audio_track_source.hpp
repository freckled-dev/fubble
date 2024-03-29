#ifndef UUID_ED1C033F_262D_4175_8DC7_FA4A5DF771BB
#define UUID_ED1C033F_262D_4175_8DC7_FA4A5DF771BB

#include "audio_track.hpp"
#include <api/scoped_refptr.h>

namespace rtc {
namespace google {
// audio track on sending side
// TODO implementation looks quite like audio_track_sink. unify
class audio_track_source : public audio_track {
public:
  audio_track_source(rtc::scoped_refptr<webrtc::AudioTrackInterface>,
                     rtc::audio_source &source);
  ~audio_track_source();

  webrtc::AudioTrackInterface &get_native_audio_track();
  rtc::audio_source &get_source() override;
  void set_volume(double) override;
  void set_enabled(bool) override;
  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
  native_track() const override;

protected:
  rtc::scoped_refptr<webrtc::AudioTrackInterface> native_track_;
  audio_source &source;
};
} // namespace google
} // namespace rtc

#endif
