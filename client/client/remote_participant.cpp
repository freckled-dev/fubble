#include "remote_participant.hpp"
#include "peer.hpp"
#include "rtc/google/video_track_source.hpp"

using namespace client;

remote_participant::remote_participant(
    std::unique_ptr<peer> peer_,
    const session::participant &session_participant)
    : peer_(std::move(peer_)), session_participant(session_participant) {
  peer_->rtc_connection().on_track.connect(
      [this](auto track) { on_track(track); });
}

remote_participant::~remote_participant() = default;

std::string remote_participant::get_id() const {
  return session_participant.id;
}

std::string remote_participant::get_name() const {
  return session_participant.name;
}

void remote_participant::on_track(rtc::track_ptr track) {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "on_track";
  auto video_track =
      std::dynamic_pointer_cast<rtc::google::video_source>(track);
  if (!video_track) {
    // TODO support audio
    BOOST_ASSERT(false && "not implemented");
    return;
  }
  on_video_added(video_track);
}
