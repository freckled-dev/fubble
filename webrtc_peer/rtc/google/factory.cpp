#include "factory.hpp"
#include "asio_signalling_thread.hpp"
#include "audio_source.hpp"
#include "audio_track_source.hpp"
#include "connection.hpp"
#include "uuid.hpp"
#include "video_source.hpp"
#include "video_track.hpp"
#include "video_track_source.hpp"
#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>

using namespace rtc::google;

factory::factory(rtc::Thread &signaling_thread)
    : signaling_thread(&signaling_thread) {
  instance_members();
}

factory::factory() { instance_members(); }

factory::~factory() = default;

std::unique_ptr<rtc::connection> factory::create_connection() {
  webrtc::PeerConnectionInterface::RTCConfiguration configuration;
  // configuration.enable_dtls_srtp = false;
  configuration.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
#if 1
  webrtc::PeerConnectionInterface::IceServer ice_server;
  ice_server.uri = "stun:stun.l.google.com:19302";
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
  return std::make_unique<audio_track_source>(native);
}

rtc::Thread &factory::get_signaling_thread() const { return *signaling_thread; }

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
  audio_decoder = webrtc::CreateBuiltinAudioDecoderFactory();
  audio_encoder = webrtc::CreateBuiltinAudioEncoderFactory();
}

void factory::instance_video() {
  // WATCHOUT. `instance_factory` will move them inside factory!
  // weird api design.
  video_decoder = webrtc::CreateBuiltinVideoDecoderFactory();
  video_encoder = webrtc::CreateBuiltinVideoEncoderFactory();
}

void factory::instance_factory() {
  factory_ = webrtc::CreatePeerConnectionFactory(
      network_thread.get(), worker_thread.get(), signaling_thread, default_adm,
      audio_encoder, audio_decoder, std::move(video_encoder),
      std::move(video_decoder), audio_mixer, audio_processing);
  if (factory_)
    return;
  throw std::runtime_error("could not instance peer factory");
}
