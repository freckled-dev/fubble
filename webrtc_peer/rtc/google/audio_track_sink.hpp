#ifndef UUID_4E20BFCB_71BE_4BB3_9D3A_8C30209AF1D1
#define UUID_4E20BFCB_71BE_4BB3_9D3A_8C30209AF1D1

#include "audio_source.hpp"
#include "audio_track.hpp"
#include <api/media_stream_interface.h>

namespace rtc {
namespace google {
class audio_track_sink : public audio_track, public audio_source {
public:
  audio_track_sink(
      const rtc::scoped_refptr<webrtc::AudioTrackInterface> &track);

  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
  native_track() const override;

protected:
  const rtc::scoped_refptr<webrtc::AudioTrackInterface> track;
};
} // namespace google
} // namespace rtc

#endif
