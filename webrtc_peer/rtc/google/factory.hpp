#ifndef RTC_GOOGLE_FACTORY_HPP
#define RTC_GOOGLE_FACTORY_HPP

#include "rtc/logger.hpp"
#include <api/create_peerconnection_factory.h>
#include <boost/asio/io_context.hpp>
#include <boost/predef/os/windows.h>
#include <memory>
#if BOOST_OS_WINDOWS
#include <modules/audio_device/win/core_audio_utility_win.h>
#endif

namespace rtc {
namespace google {
class video_source;
class video_track;
class audio_source;
class audio_track;
class audio_devices;
class connection;
struct settings {
  bool use_ip_v6{true};
#if BOOST_OS_WINDOWS
  bool windows_use_core_audio2{false}; // windows core audio 2 is experimental
#endif
};
class factory {
public:
  explicit factory(const settings &settings_, rtc::Thread &signaling_thread);
  // TODO remove default constructor
  factory();
  ~factory();

  std::unique_ptr<connection> create_connection();
  std::unique_ptr<video_track>
  create_video_track(const std::shared_ptr<video_source> &source);

  std::unique_ptr<audio_track> create_audio_track(audio_source &source);

  webrtc::PeerConnectionFactoryInterface &get_native() const;

  rtc::Thread &get_signaling_thread() const;
  audio_devices &get_audio_devices();

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
#if BOOST_OS_WINDOWS
  std::unique_ptr<webrtc::webrtc_win::ScopedCOMInitializer> com_initializer_;
#endif
  rtc::scoped_refptr<webrtc::AudioDeviceModule> audio_device_module;
  std::unique_ptr<webrtc::VideoDecoderFactory> video_decoder;
  std::unique_ptr<webrtc::VideoEncoderFactory> video_encoder;
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory_;
  std::unique_ptr<audio_devices> audio_devices_;
};
} // namespace google
} // namespace rtc

#endif
