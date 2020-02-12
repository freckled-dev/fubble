#include "add_audio_to_connection.hpp"
#include "rtc/google/audio_track.hpp"
#include "rtc/google/connection.hpp"

using namespace client;

add_audio_to_connection::add_audio_to_connection(
    rtc::google::factory &rtc_factory, rtc::google::audio_source &source)
    : rtc_factory(rtc_factory), source(source) {}

add_audio_to_connection::~add_audio_to_connection() = default;

void add_audio_to_connection::add_to_connection(rtc::connection &connection) {
  std::unique_ptr<rtc::google::audio_track> audio_track =
      rtc_factory.create_audio_track(source);
  std::shared_ptr<rtc::google::audio_track> audio_track_shared =
      std::move(audio_track);
  connection.add_track(audio_track_shared);
}
