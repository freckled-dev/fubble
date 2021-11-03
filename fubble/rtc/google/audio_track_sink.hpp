#ifndef UUID_4E20BFCB_71BE_4BB3_9D3A_8C30209AF1D1
#define UUID_4E20BFCB_71BE_4BB3_9D3A_8C30209AF1D1

#include "audio_track.hpp"
#include <api/media_stream_interface.h>

namespace rtc {
namespace google {
class audio_track_sink : public audio_track {
public:
  audio_track_sink(
      const rtc::scoped_refptr<webrtc::AudioTrackInterface> &track);
  ~audio_track_sink();

  webrtc::AudioTrackInterface &get_native_audio_track();
  audio_source &get_source() override;
  void set_volume(double) override;
  void set_enabled(bool) override;
  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
  native_track() const override;

protected:
  const rtc::scoped_refptr<webrtc::AudioTrackInterface> native_track_;
  std::unique_ptr<audio_source> source;
};
} // namespace google
} // namespace rtc

#endif
