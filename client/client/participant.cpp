#include "participant.hpp"
#include "peer.hpp"
#include "rtc/google/video_track_source.hpp"

using namespace client;

participant::participant(std::unique_ptr<peer> peer_,
                         const session::participant &session_participant)
    : peer_(std::move(peer_)), session_participant(session_participant) {
  peer_->rtc_connection().on_track.connect(
      [this](auto track) { on_track(track); });
  peer_->connect(session_participant.id); // TODO this won't work!!!
}

participant::~participant() = default;

std::string participant::get_name() const { return session_participant.name; }

void participant::on_track(rtc::track_ptr track) {
  auto video_track =
      std::dynamic_pointer_cast<rtc::google::video_source>(track);
  if (!video_track) {
    // TODO support audio
    BOOST_ASSERT(false && "not implemented");
    return;
  }
  on_video_added(video_track);
}
