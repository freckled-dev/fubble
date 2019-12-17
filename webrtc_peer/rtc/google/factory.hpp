#ifndef RTC_GOOGLE_FACTORY_HPP
#define RTC_GOOGLE_FACTORY_HPP

#include <api/create_peerconnection_factory.h>
#include <memory>

namespace rtc {
class connection;
namespace google {
class video_track;
class video_source;
class factory {
public:
  factory();
  ~factory();

  std::unique_ptr<connection> create_connection();
  std::unique_ptr<video_track>
  create_video_track(const std::shared_ptr<video_source> &source);

private:
  void instance_threads();
  void instance_audio();
  void instance_video();
  void instance_factory();

  std::unique_ptr<rtc::Thread> network_thread;
  std::unique_ptr<rtc::Thread> worker_thread;
  std::unique_ptr<rtc::Thread> signaling_thread;
  rtc::scoped_refptr<webrtc::AudioDeviceModule> default_adm;
  rtc::scoped_refptr<webrtc::AudioMixer> audio_mixer;
  rtc::scoped_refptr<webrtc::AudioProcessing> audio_processing;
  rtc::scoped_refptr<webrtc::AudioDecoderFactory> audio_decoder;
  rtc::scoped_refptr<webrtc::AudioEncoderFactory> audio_encoder;
  std::unique_ptr<webrtc::VideoDecoderFactory> video_decoder;
  std::unique_ptr<webrtc::VideoEncoderFactory> video_encoder;
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory_;
};
} // namespace google
} // namespace rtc

#endif
