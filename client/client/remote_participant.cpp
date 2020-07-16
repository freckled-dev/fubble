#include "remote_participant.hpp"
#include "client/factory.hpp"
#include "client/peer.hpp"
#include "client/tracks_adder.hpp"
#include "rtc/google/audio_track_sink.hpp"
#include "rtc/google/video_track_sink.hpp"

using namespace client;

remote_participant::remote_participant(factory &factory_,
                                       std::unique_ptr<peer> peer_moved,
                                       matrix::user &matrix_participant,
                                       tracks_adder &tracks_adder_)
    : participant(matrix_participant), factory_(factory_),
      tracks_adder_(tracks_adder_), peer_(std::move(peer_moved)) {
  tracks_adder_.add_connection(peer_->rtc_connection());
  peer_->rtc_connection().on_track.connect(
      [this](auto track) { on_track(track); });
  // TODO track removal!
}

remote_participant::~remote_participant() {
  tracks_adder_.remove_connection(peer_->rtc_connection());
}

boost::future<void> remote_participant::close() { return peer_->close(); }

remote_participant::videos_type remote_participant::get_videos() const {
  return videos;
}

remote_participant::audios_type remote_participant::get_audios() const {
  return audios;
}

void remote_participant::on_track(rtc::track_ptr track) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  auto video_track =
      std::dynamic_pointer_cast<rtc::google::video_source>(track);
  if (video_track)
    return on_video_track(video_track);
  auto audio_track =
      std::dynamic_pointer_cast<rtc::google::audio_track_sink>(track);
  if (audio_track)
    return on_audio_track(audio_track);
  BOOST_LOG_SEV(logger, logging::severity::warning) << "unhandled track";
  BOOST_ASSERT(false);
}

void remote_participant::on_audio_track(
    std::shared_ptr<rtc::google::audio_track_sink> audio_track) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;

  rtc::google::audio_source &source = audio_track->get_source();
  audios.emplace_back(&source);
  on_audio_added(source);
}

void remote_participant::on_video_track(
    rtc::google::video_source_ptr video_track) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  videos.emplace_back(video_track.get());
  on_video_added(video_track);
}

