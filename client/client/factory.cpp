#include "factory.hpp"
#include "audio_track_information_listener.hpp"

using namespace client;

factory::factory(boost::asio::io_context &context) : context(context) {}

std::unique_ptr<audio_track_information_listener>
factory::create_audio_track_information_listener(
    rtc::google::audio_track &track) {
  return std::make_unique<audio_track_information_listener>(context, track);
}

