#include "device_creator.hpp"
#include "device.hpp"
#include "factory.hpp"

using namespace rtc::google::capture::audio;

device_creator::device_creator(factory &factory_) : factory_(factory_) {}

std::unique_ptr<device> device_creator::create() {
  webrtc::PeerConnectionFactoryInterface &peer_factory = factory_.get_native();
  cricket::AudioOptions options;
  rtc::scoped_refptr<webrtc::AudioSourceInterface> native_source =
      peer_factory.CreateAudioSource(options);
  BOOST_ASSERT(native_source);
  return std::make_unique<device>(native_source);
}

