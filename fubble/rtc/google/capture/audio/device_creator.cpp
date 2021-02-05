#include "device_creator.hpp"
#include "device.hpp"
#include "factory.hpp"

using namespace rtc::google::capture::audio;

device_creator::device_creator(rtc::google::factory &factory_)
    : factory_(factory_) {}

std::unique_ptr<device> device_creator::create() {
  webrtc::PeerConnectionFactoryInterface &peer_factory = factory_.get_native();
  cricket::AudioOptions options;
  // options.auto_gain_control = false;
  // options.noise_suppression = false;
  const auto& settings_ = factory_.get_settings().audio_;
  options.echo_cancellation = settings_.enable_echo_canceller;
  options.auto_gain_control = settings_.enable_gain_controller;
  options.highpass_filter = settings_.enable_high_pass_filter;
  // options.voice_detection = settings_.enable_voice_detection; // TODO set to offer, for some reason
  options.noise_suppression = settings_.enable_noise_suppression;
  options.typing_detection = settings_.enable_typing_detection;
  options.audio_jitter_buffer_enable_rtx_handling = settings_.jitter_buffer_enable_rtx_handling;
  options.audio_jitter_buffer_max_packets = settings_.jitter_buffer_max_packets;
  options.audio_jitter_buffer_min_delay_ms = settings_.jitter_buffer_min_delay_ms;
  options.audio_jitter_buffer_fast_accelerate = settings_.jitter_buffer_fast_accelerate;

  rtc::scoped_refptr<webrtc::AudioSourceInterface> native_source =
      peer_factory.CreateAudioSource(options);
  BOOST_ASSERT(native_source);
  if (native_source)
    return std::make_unique<device>(native_source);
  throw std::runtime_error("could not initialise any audio device");
}
