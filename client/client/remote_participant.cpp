#include "remote_participant.hpp"
#include "peer.hpp"
#include "rtc/google/video_track_source.hpp"

using namespace client;

remote_participant::remote_participant(
    std::unique_ptr<peer> peer_moved,
    const session::participant &session_participant)
    : peer_(std::move(peer_moved)), session_participant(session_participant) {
  peer_->rtc_connection().on_track.connect(
      [this](auto track) { on_track(track); });
  // TODO track removal!
}

remote_participant::~remote_participant() = default;

std::string remote_participant::get_id() const {
  return session_participant.id;
}

std::string remote_participant::get_name() const {
  return session_participant.name;
}

void remote_participant::update(const session::participant &update) {
  BOOST_ASSERT(update.id == session_participant.id);
  if (session_participant.name == update.name)
    return;
  session_participant = update;
  on_name_changed(session_participant.name);
}

remote_participant::videos_type remote_participant::get_videos() const {
  return videos;
}

void remote_participant::on_track(rtc::track_ptr track) {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "on_track";
  auto video_track =
      std::dynamic_pointer_cast<rtc::google::video_source>(track);
  if (!video_track) {
    // TODO support audio
    // BOOST_ASSERT(false && "not implemented");
    return;
  }
  videos.emplace_back(video_track.get());
  on_video_added(video_track);
}