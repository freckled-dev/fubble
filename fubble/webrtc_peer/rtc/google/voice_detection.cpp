#include "voice_detection.hpp"
#include "rtc/google/audio_data.hpp"
#include "rtc/logger.hpp"
#include <boost/assert.hpp>
#include <common_audio/vad/include/webrtc_vad.h>
#include <modules/audio_processing/audio_buffer.h>
#include <modules/audio_processing/include/audio_processing.h>
#include <modules/audio_processing/voice_detection.h>

using namespace rtc::google;

namespace {
class voice_detection_impl : public voice_detection {
public:
  voice_detection_impl() {
    instance = WebRtcVad_Create();
    check_result(WebRtcVad_Init(instance));
    check_result(WebRtcVad_set_mode(instance, 3)); // [0, 3]
  }

  ~voice_detection_impl() { WebRtcVad_Free(instance); }

  bool detect(const audio_data &data) override {
    // TODO what if there're two channels?
    BOOST_ASSERT(0 == WebRtcVad_ValidRateAndFrameLength(data.sample_rate,
                                                        data.number_of_frames));
    int result =
        WebRtcVad_Process(instance, data.sample_rate,
                          static_cast<const std::int16_t *>(data.audio_data),
                          data.number_of_frames);
    BOOST_ASSERT(result != -1);
#if 0
    BOOST_LOG_SEV(logger, logging::severity::debug) << "result:" << result;
#endif
    return result == 1;
#if 0
  {
    if (sample_rate != data.sample_rate) {
      detector = std::make_unique<webrtc::VoiceDetection>(
          data.sample_rate, webrtc::VoiceDetection::kHighLikelihood);
      sample_rate = data.sample_rate;
    }
#if 0
    webrtc::AudioBuffer buffer{data.sample_rate,      data.number_of_channels,
                               data.number_of_frames, data.number_of_channels,
                               data.number_of_frames, data.number_of_channels};
#else
    webrtc::AudioBuffer buffer{data.number_of_frames, data.number_of_channels,
                               data.number_of_frames, data.number_of_channels,
                               data.number_of_frames};
#endif
    buffer.CopyFrom(
        static_cast<const std::int16_t *>(data.audio_data),
        webrtc::StreamConfig{data.sample_rate, data.number_of_channels});
    bool voice_detected = detector->ProcessCaptureAudio(&buffer);
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "voice_detected:" << voice_detected;
  }
#endif
  }
  void check_result(int result) {
    // on error result is always -1
    BOOST_ASSERT(result == 0);
    if (result == 0)
      return;
    BOOST_LOG_SEV(logger, logging::severity::error) << __FUNCTION__;
  }

protected:
  rtc::logger logger{"voice_detection_impl"};
  VadInst *instance{};
#if 0
  std::unique_ptr<webrtc::VoiceDetection> detector;
  int sample_rate{};
#endif
};
} // namespace

std::unique_ptr<voice_detection> voice_detection::create() {
  return std::make_unique<voice_detection_impl>();
}
