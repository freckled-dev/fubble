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
  rtc::scoped_refptr<webrtc::AudioSourceInterface> native_source =
      peer_factory.CreateAudioSource(options);
  BOOST_ASSERT(native_source);
  if (native_source)
    return std::make_unique<device>(native_source);
  throw std::runtime_error("could not initialise any audio device");
}
