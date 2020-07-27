#include "add_audio_to_connection.hpp"
#include "rtc/google/audio_track.hpp"
#include "rtc/google/connection.hpp"

using namespace client;

add_audio_to_connection::add_audio_to_connection(
    rtc::google::factory &rtc_factory, rtc::google::audio_source &source)
    : rtc_factory(rtc_factory), source(source) {
  audio_track = rtc_factory.create_audio_track(source);
}

add_audio_to_connection::~add_audio_to_connection() = default;

void add_audio_to_connection::add_to_connection(rtc::connection &connection) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  connection.add_track(audio_track);
}

void add_audio_to_connection::remove_from_connection(
    rtc::connection &connection) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  connection.remove_track(audio_track);
}

std::shared_ptr<rtc::google::audio_track> add_audio_to_connection::get_track() {
  return audio_track;
}
