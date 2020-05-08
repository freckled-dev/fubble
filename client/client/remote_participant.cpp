#include "remote_participant.hpp"
#include "peer.hpp"
#include "rtc/google/video_track_source.hpp"

using namespace client;

remote_participant::remote_participant(std::unique_ptr<peer> peer_moved,
                                       matrix::user &matrix_participant)
    : participant(matrix_participant), peer_(std::move(peer_moved)) {
  peer_->rtc_connection().on_track.connect(
      [this](auto track) { on_track(track); });
  // TODO track removal!
}

remote_participant::~remote_participant() = default;

boost::future<void> remote_participant::close() { return peer_->close(); }

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
