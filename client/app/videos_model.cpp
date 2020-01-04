#include "videos_model.hpp"
#include "peer.hpp"
#include "peers.hpp"
#include "rtc/google/video_source.hpp"
#include "rtc/track.hpp"
#include <QCoreApplication>

using namespace client;

videos_model::videos_model(peers &peers_) : peers_(peers_) {
  peers_.on_added.connect([this]() { on_peer_added(); });
  own_video = new ui::frame_provider_google_video_source(this);
}

void videos_model::on_peer_added() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "on_peer_added";
  peers::peer_ptr peer_ = peers_.get_all().back();
  BOOST_ASSERT(peer_);
  peer_->rtc_connection().on_track.connect(
      [this](const auto &track) { on_track(track); });
}

ui::frame_provider_google_video_source *videos_model::get_video_source() const {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "get_video_source()";
  return video_source;
}

ui::frame_provider_google_video_source *videos_model::get_own_video() const {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "get_own_video()";
  return own_video;
}

void videos_model::on_track(const rtc::track_ptr &track) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "videos_model::on_track";
  rtc::google::video_source *casted =
      dynamic_cast<rtc::google::video_source *>(track.get());
  BOOST_ASSERT(casted);
  if (casted == nullptr)
    return;
  video_source = new ui::frame_provider_google_video_source(this);
  video_source->moveToThread(QCoreApplication::instance()->thread());
  video_source->set_source(casted);
  BOOST_LOG_SEV(logger, logging::severity::debug) << "calling signal";
  video_source_changed(video_source);
}
