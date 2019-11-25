#include "connection_creator.hpp"
#include "connection.hpp"
#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>

using namespace rtc::google;

connection_creator::connection_creator() {
  instance_threads();
  instance_audio();
  instance_video();
  instance_factory();
}

std::unique_ptr<connection> connection_creator::operator()() {
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
      factory->CreatePeerConnection(configuration, std::move(dependencies));
  if (!native)
    throw std::runtime_error("!peer_connection");
  result->initialise(native);
  return result;
}

void connection_creator::instance_threads() {
  network_thread = rtc::Thread::CreateWithSocketServer();
  worker_thread = rtc::Thread::Create();
  signaling_thread = rtc::Thread::Create();
  network_thread->Start();
  worker_thread->Start();
  signaling_thread->Start();
}

void connection_creator::instance_audio() {
  audio_decoder = webrtc::CreateBuiltinAudioDecoderFactory();
  audio_encoder = webrtc::CreateBuiltinAudioEncoderFactory();
}

void connection_creator::instance_video() {
  // WATCHOUT. `instance_factory` will move them inside factory!
  // weird api design.
  video_decoder = webrtc::CreateBuiltinVideoDecoderFactory();
  video_encoder = webrtc::CreateBuiltinVideoEncoderFactory();
}

void connection_creator::instance_factory() {
  factory = webrtc::CreatePeerConnectionFactory(
      network_thread.get(), worker_thread.get(), signaling_thread.get(),
      default_adm, audio_encoder, audio_decoder, std::move(video_encoder),
      std::move(video_decoder), audio_mixer, audio_processing);
  if (factory)
    return;
  throw std::runtime_error("could not instance peer factory");
}
