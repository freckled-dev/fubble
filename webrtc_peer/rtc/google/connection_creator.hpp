#ifndef RTC_GOOGLE_CONNECTION_CREATOR_HPP
#define RTC_GOOGLE_CONNECTION_CREATOR_HPP

#include <api/create_peerconnection_factory.h>
#include <memory>

namespace rtc::google {
class connection;
class connection_creator {
public:
  connection_creator();

  std::unique_ptr<connection> operator()();

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
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory;
};
} // namespace rtc::google

#endif
