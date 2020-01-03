#include "videos_model.hpp"
#include "peer.hpp"
#include "peers.hpp"
#include "rtc/google/video_source.hpp"
#include "rtc/track.hpp"

using namespace client;

videos_model::videos_model(peers &peers_) : peers_(peers_) {
  video_source = new ui::frame_provider_google_video_source();
  peers_.on_added.connect([this]() { on_peer_added(); });
}

void videos_model::on_peer_added() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "on_peer_added";
  peers::peer_ptr peer_ = peers_.get_all().back();
  BOOST_ASSERT(peer_);
  peer_->rtc_connection().on_track.connect(
      [this](const auto &track) { on_track(track); });
}

void videos_model::on_track(const rtc::track_ptr &track) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "videos_model::on_track";
  rtc::google::video_source *casted =
      dynamic_cast<rtc::google::video_source *>(track.get());
  BOOST_ASSERT(casted);
  if (casted == nullptr)
    return;
  video_source->set_source(casted);
  // video_source_changed(video_source);
}
