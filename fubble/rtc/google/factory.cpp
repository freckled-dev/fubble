#include "factory.hpp"
#include "asio_signaling_thread.hpp"
#include "audio_devices.hpp"
#include "audio_source.hpp"
#include "audio_track_source.hpp"
#include "connection.hpp"
#include "fubble/utils/uuid.hpp"
#include "video_source.hpp"
#include "video_track.hpp"
#include "video_track_source.hpp"
#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/audio_codecs/opus_audio_encoder_factory.h>
#include <api/task_queue/default_task_queue_factory.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>

using namespace rtc::google;

factory::factory(const settings &settings_, rtc::Thread &signaling_thread)
    : settings_(settings_), signaling_thread(&signaling_thread) {
  instance_members();
}

factory::factory() : settings_{} { instance_members(); }

factory::~factory() {
  if (!audio_device_module)
    return;
  worker_thread->Invoke<void>(RTC_FROM_HERE,
                              [this] { audio_device_module = nullptr; });
}

std::unique_ptr<connection> factory::create_connection() {
  webrtc::PeerConnectionInterface::RTCConfiguration configuration;
  configuration.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
  if (!settings_.use_ip_v6) {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "setting `disable_ipv6` to `true`";
    configuration.disable_ipv6 = true;
  }
  configuration.disable_ipv6_on_wifi = settings_.disable_ipv6_on_wifi;
  configuration.audio_jitter_buffer_max_packets =
      settings_.audio_.jitter_buffer_max_packets;
  configuration.audio_jitter_buffer_fast_accelerate =
      settings_.audio_.jitter_buffer_fast_accelerate;
  configuration.audio_jitter_buffer_min_delay_ms =
      settings_.audio_.jitter_buffer_min_delay_ms;
  configuration.audio_jitter_buffer_enable_rtx_handling =
      settings_.audio_.jitter_buffer_enable_rtx_handling;
#if 1
  webrtc::PeerConnectionInterface::IceServer ice_server;
  ice_server.uri = "turn:fubble.io";
  ice_server.username = "fubble";
  ice_server.password = "A2xzNc5xqO8MlSpOxIXt";
  configuration.servers.push_back(ice_server);
#endif
  auto result = std::make_unique<connection>();
  webrtc::PeerConnectionDependencies dependencies{result.get()};
  auto native =
      factory_->CreatePeerConnection(configuration, std::move(dependencies));
  if (!native)
    throw std::runtime_error("!peer_connection");
  result->initialise(native);
  return result;
}

std::unique_ptr<video_track>
factory::create_video_track(const std::shared_ptr<video_source> &source) {
  auto label = uuid::generate();
  rtc::scoped_refptr<video_track_source::adapter> source_adapter =
      new rtc::RefCountedObject<video_track_source::adapter>;
  rtc::scoped_refptr<webrtc::VideoTrackInterface> native(
      factory_->CreateVideoTrack(label, source_adapter.get()));
  assert(native);
  if (!native)
    throw std::runtime_error("could not create video track");
  // TODO adapter shall take source, not track
  auto result =
      std::make_unique<video_track_source>(native, source_adapter, source);
  return result;
}

std::unique_ptr<audio_track> factory::create_audio_track(audio_source &source) {
  auto label = uuid::generate();
  webrtc::AudioSourceInterface &native_source = source.get_native();
  rtc::scoped_refptr<webrtc::AudioTrackInterface> native =
      factory_->CreateAudioTrack(label, &native_source);
  BOOST_ASSERT(native->GetSource() == &native_source);
  return std::make_unique<audio_track_source>(native, source);
}

rtc::Thread &factory::get_signaling_thread() const { return *signaling_thread; }

audio_devices &factory::get_audio_devices() { return *audio_devices_; }

const settings &factory::get_settings() const { return settings_; }

void factory::instance_members() {
  instance_threads();
  instance_audio();
  instance_video();
  instance_factory();
}

webrtc::PeerConnectionFactoryInterface &factory::get_native() const {
  return *factory_;
}

void factory::instance_threads() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "instance_threads";
  network_thread = rtc::Thread::CreateWithSocketServer();
  network_thread->Start();
  worker_thread = rtc::Thread::Create();
  worker_thread->Start();
  if (signaling_thread)
    return;
  signaling_thread_own = rtc::Thread::Create();
  signaling_thread_own->Start();
  signaling_thread = signaling_thread_own.get();
}

void factory::instance_audio() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "instance_audio";
  audio_decoder = webrtc::CreateBuiltinAudioDecoderFactory();
#if 1
  audio_encoder = webrtc::CreateBuiltinAudioEncoderFactory();
#else
  audio_encoder = webrtc::CreateOpusAudioEncoderFactory();
#endif
  instance_audio_processing();
  instance_audio_device_module();
  audio_devices_ =
      std::make_unique<audio_devices>(*worker_thread, *audio_device_module);
}

void factory::instance_audio_processing() {
  webrtc::AudioProcessingBuilder builder;
  audio_processing = builder.Create();
  // do not apply config. will be overwritten with the options from
  // device_creator audio_processing->ApplyConfig(config);
}

namespace {
webrtc::AudioDeviceModule::AudioLayer
cast_audio_layer(settings::audio::layer cast) {
  switch (cast) {
  case settings::audio::layer::default_:
    return webrtc::AudioDeviceModule::kPlatformDefaultAudio;
  case settings::audio::layer::windows_core:
    return webrtc::AudioDeviceModule::kWindowsCoreAudio;
  case settings::audio::layer::windows_core2:
    return webrtc::AudioDeviceModule::kWindowsCoreAudio2;
  case settings::audio::layer::linux_alsa:
    return webrtc::AudioDeviceModule::kLinuxAlsaAudio;
  case settings::audio::layer::linux_pulse:
    return webrtc::AudioDeviceModule::kLinuxPulseAudio;
  case settings::audio::layer::android_java:
    return webrtc::AudioDeviceModule::kAndroidJavaAudio;
  case settings::audio::layer::android_open_sles:
    return webrtc::AudioDeviceModule::kAndroidOpenSLESAudio;
  case settings::audio::layer::android_java_input_and_open_sles_output:
    return webrtc::AudioDeviceModule::kAndroidJavaInputAndOpenSLESOutputAudio;
  case settings::audio::layer::android_aaudio:
    return webrtc::AudioDeviceModule::kAndroidAAudioAudio;
  case settings::audio::layer::android_java_input_and_aaudio_output:
    return webrtc::AudioDeviceModule::kAndroidJavaInputAndAAudioOutputAudio;
  case settings::audio::layer::dummy:
    return webrtc::AudioDeviceModule::kDummyAudio;
  }
  BOOST_ASSERT(false);
}
} // namespace

void factory::instance_audio_device_module() {
  task_queue_factory = webrtc::CreateDefaultTaskQueueFactory();
  audio_device_module = worker_thread->Invoke<decltype(audio_device_module)>(
      RTC_FROM_HERE, [this]() -> rtc::scoped_refptr<webrtc::AudioDeviceModule> {
        return webrtc::AudioDeviceModule::Create(
            cast_audio_layer(settings_.audio_.layer_),
            task_queue_factory.get());
      });
  BOOST_ASSERT(audio_device_module);
}

void factory::instance_video() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "instance_video";
  // WATCHOUT. `instance_factory` will move them inside factory!
  // weird api design.
  video_decoder = webrtc::CreateBuiltinVideoDecoderFactory();
  video_encoder = webrtc::CreateBuiltinVideoEncoderFactory();
  for (const auto format : video_encoder->GetSupportedFormats()) {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << "supported video_format: " << format.ToString();
  }
}

void factory::instance_factory() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "instance_factory";
  factory_ = webrtc::CreatePeerConnectionFactory(
      network_thread.get(), worker_thread.get(), signaling_thread,
      audio_device_module, audio_encoder, audio_decoder,
      std::move(video_encoder), std::move(video_decoder), audio_mixer,
      audio_processing);
  if (factory_)
    return;
  BOOST_LOG_SEV(logger, logging::severity::error)
      << "could not instance peer factory!";
  throw std::runtime_error("could not instance peer factory");
}
