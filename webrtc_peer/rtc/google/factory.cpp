#include "factory.hpp"
#include "connection.hpp"
#include "video_source.hpp"
#include "video_track.hpp"
#include "video_track_source.hpp"
#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

using namespace rtc::google;

factory::factory() {
  instance_threads();
  instance_audio();
  instance_video();
  instance_factory();
}

factory::~factory() = default;

std::unique_ptr<rtc::connection> factory::create_connection() {
  webrtc::PeerConnectionInterface::RTCConfiguration configuration;
#if 0
  configuration.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
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
  auto label = boost::uuids::random_generator()();
  auto label_string = boost::uuids::to_string(label);
  rtc::scoped_refptr<video_track_source::adapter> source_adapter =
      new rtc::RefCountedObject<video_track_source::adapter>;
  rtc::scoped_refptr<webrtc::VideoTrackInterface> native(
      factory_->CreateVideoTrack(label_string, source_adapter.get()));
  assert(native);
  if (!native)
    throw std::runtime_error("could not create video track");
  auto result =
      std::make_unique<video_track_source>(native, source_adapter, source);
  return result;
}

void factory::instance_threads() {
  network_thread = rtc::Thread::CreateWithSocketServer();
  worker_thread = rtc::Thread::Create();
  signaling_thread = rtc::Thread::Create();
  network_thread->Start();
  worker_thread->Start();
  signaling_thread->Start();
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
      network_thread.get(), worker_thread.get(), signaling_thread.get(),
      default_adm, audio_encoder, audio_decoder, std::move(video_encoder),
      std::move(video_decoder), audio_mixer, audio_processing);
  if (factory_)
    return;
  throw std::runtime_error("could not instance peer factory");
}