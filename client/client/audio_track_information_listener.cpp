#include "audio_track_information_listener.hpp"
#include "rtc/google/audio_track.hpp"
#include <api/media_stream_interface.h>

using namespace client;

audio_track_information_listener::audio_track_information_listener(
    boost::asio::io_context &context, rtc::google::audio_track &track)
    : context(context), track(track) {
  timer.start([this] { update(); });
}

void audio_track_information_listener::update() {
  // TODO remove log entry
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  rtc::google::audio_track::stats stats = track.get_stats();
  if (stats.rms_dbfs) {
    rms_dbfs = stats.rms_dbfs.value();
    BOOST_LOG_SEV(logger, logging::severity::debug) // TODO change to trace
        << "rms_dbfs:" << rms_dbfs;
  }
  if (stats.voice_detected != voice_detected) {
    voice_detected = stats.voice_detected;
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "voice_detected:" << voice_detected;
  }
}
