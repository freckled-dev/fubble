#include "remote_participant.hpp"
#include "client/factory.hpp"
#include "client/peer.hpp"
#include "client/tracks_adder.hpp"
#include "matrix/user.hpp"
#include "rtc/google/audio_track_sink.hpp"
#include "rtc/google/video_track_sink.hpp"
#include <fmt/format.h>

using namespace client;

remote_participant::remote_participant(factory &factory_,
                                       std::unique_ptr<peer> peer_moved,
                                       matrix::user &matrix_participant,
                                       tracks_adder &tracks_adder_)
    : participant(matrix_participant), logger{fmt::format(
                                           "remote_participant:{}",
                                           matrix_participant.get_id())},
      factory_(factory_), tracks_adder_(tracks_adder_),
      peer_(std::move(peer_moved)) {
  tracks_adder_.add_connection(peer_->rtc_connection());
  peer_->rtc_connection().on_track_added.connect(
      [this](auto track) { on_track_added(track); });
  peer_->rtc_connection().on_track_removed.connect(
      [this](auto track) { on_track_removed(track); });
}

remote_participant::~remote_participant() {
  tracks_adder_.remove_connection(peer_->rtc_connection());
}

boost::future<void> remote_participant::close() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  return peer_->close();
}

remote_participant::videos_type remote_participant::get_videos() const {
  return videos;
}

remote_participant::audios_type remote_participant::get_audios() const {
  return audios;
}

void remote_participant::on_track_added(rtc::track_ptr track) {
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

void remote_participant::on_track_removed(rtc::track_ptr track) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  auto video_track =
      std::dynamic_pointer_cast<rtc::google::video_source>(track);
  if (video_track)
    return on_video_track_removed(video_track);
  auto audio_track =
      std::dynamic_pointer_cast<rtc::google::audio_track_sink>(track);
  if (audio_track)
    return on_audio_track_removed(audio_track);
  BOOST_LOG_SEV(logger, logging::severity::warning) << "unhandled track";
  BOOST_ASSERT(false);
}

void remote_participant::on_audio_track(
    std::shared_ptr<rtc::google::audio_track_sink> audio_track) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;

  audios.emplace_back(audio_track.get());
  on_audio_added(*audio_track);
}

void remote_participant::on_audio_track_removed(
    std::shared_ptr<rtc::google::audio_track_sink> audio_track) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  auto found = std::find_if(audios.cbegin(), audios.cend(), [&](auto check) {
    return check == audio_track.get();
  });
  audios.erase(found);
  on_audio_removed(*audio_track);
}

void remote_participant::on_video_track(
    rtc::google::video_source_ptr video_track) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  videos.emplace_back(video_track.get());
  on_video_added(video_track);
}

void remote_participant::on_video_track_removed(
    rtc::google::video_source_ptr video_track) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  auto found = std::find_if(videos.cbegin(), videos.cend(),
                            [&](auto &check) { return check == video_track; });
  videos.erase(found);
  on_video_removed(video_track);
}
