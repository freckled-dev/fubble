#ifndef RTC_GOOGLE_FACTORY_HPP
#define RTC_GOOGLE_FACTORY_HPP

#include <api/create_peerconnection_factory.h>
#include <memory>

namespace rtc {
class connection;
namespace google {
class video_track;
class video_source;
// TODO don't create a signalling thread in this class. instead use an external
// one
class factory {
public:
  factory(std::unique_ptr<rtc::Thread> signaling_thread);
  factory();
  ~factory();

  std::unique_ptr<connection> create_connection();
  std::unique_ptr<video_track>
  create_video_track(const std::shared_ptr<video_source> &source);

  rtc::Thread &get_signaling_thread() const;

private:
  void instance_members();
  void instance_threads();
  void instance_audio();
  void instance_video();
  void instance_factory();

  std::unique_ptr<rtc::Thread> network_thread;
  std::unique_ptr<rtc::Thread> worker_thread;
  // TODO replace signaling_thread with a local thread (asio) implementation
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
