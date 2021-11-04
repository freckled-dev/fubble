#ifndef RTC_GOOGLE_FACTORY_HPP
#define RTC_GOOGLE_FACTORY_HPP

#include <api/create_peerconnection_factory.h>
#include <boost/asio/io_context.hpp>
#include <fubble/rtc/connection.hpp>
#include <fubble/rtc/factory.hpp>
#include <fubble/rtc/google/settings.hpp>
#include <fubble/rtc/google/video_encoder_factory_factory.hpp>
#include <fubble/rtc/logger.hpp>
#include <fubble/utils/export.hpp>
#include <memory>

namespace rtc {
class connection;
namespace google {
class video_source;
class video_track;
class audio_source;
class audio_track;
class audio_devices;
class connection;
class FUBBLE_PUBLIC factory : public rtc::factory {
public:
  // TODO remove default constructor
  factory();
  explicit factory(std::shared_ptr<video_encoder_factory_factory>
                       video_encoder_factory_factory_,
                   const settings &settings_, rtc::Thread &signaling_thread);
  ~factory();

  std::unique_ptr<rtc::connection> create_connection() override;
  std::unique_ptr<rtc::video_track>
  create_video_track(const std::shared_ptr<rtc::video_source> &source) override;

  std::unique_ptr<rtc::audio_track>
  create_audio_track(rtc::audio_source &source) override;

  webrtc::PeerConnectionFactoryInterface &get_native() const;

  rtc::Thread &get_signaling_thread() const;
  std::shared_ptr<rtc::audio_devices> get_audio_devices() override;
  const settings &get_settings() const;

private:
  void instance_members();
  void instance_threads();
  void instance_audio();
  void instance_audio_processing();
  void instance_audio_device_module();
  void instance_video();
  void instance_factory();

  rtc::logger logger{"google::factory"};
  const settings settings_;
  const std::shared_ptr<video_encoder_factory_factory>
      video_encoder_factory_factory_;
  std::unique_ptr<rtc::Thread> network_thread;
  std::unique_ptr<rtc::Thread> worker_thread;
  // TODO replace signaling_thread with a local thread (asio) implementation
  std::unique_ptr<rtc::Thread> signaling_thread_own;
  rtc::Thread *signaling_thread{};
  std::unique_ptr<webrtc::TaskQueueFactory> task_queue_factory;
  rtc::scoped_refptr<webrtc::AudioMixer> audio_mixer;
  rtc::scoped_refptr<webrtc::AudioProcessing> audio_processing;
  rtc::scoped_refptr<webrtc::AudioDecoderFactory> audio_decoder;
  rtc::scoped_refptr<webrtc::AudioEncoderFactory> audio_encoder;
  rtc::scoped_refptr<webrtc::AudioDeviceModule> audio_device_module;
  std::unique_ptr<webrtc::VideoDecoderFactory> video_decoder;
  std::unique_ptr<webrtc::VideoEncoderFactory> video_encoder;
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory_;
  std::shared_ptr<audio_devices> audio_devices_;
};
} // namespace google
} // namespace rtc

#endif
