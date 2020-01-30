#include "participant.hpp"
#include "peer.hpp"
#include "peer_creator.hpp"
#include "rtc/google/video_track_source.hpp"

using namespace client;

participant::participant(peer_creator &peer_creator_,
                         session::participant &session_participant)
    : peer_creator_(peer_creator_), session_participant(session_participant) {
  peer = peer_creator_.create();
  peer->rtc_connection().on_track.connect(
      [this](auto track) { on_track(track); });
  peer->connect(session_participant.id); // TODO this won't work!!!
}

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
