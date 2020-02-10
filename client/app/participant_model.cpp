#include "participant_model.hpp"
#include "participant.hpp"

using namespace client;

participant_model::participant_model(participant &participant_, QObject *parent)
    : QObject(parent), participant_(participant_) {
  set_name();
  participant_.on_name_changed.connect([this](auto) { set_name(); });
  participant_.on_video_added.connect([this](auto added) {
    BOOST_ASSERT(added);
    video_added(*added);
  });
  auto videos = participant_.get_videos();
  for (auto video : videos) {
    BOOST_ASSERT(video);
    video_added(*video);
  }
}

ui::frame_provider_google_video_source *participant_model::get_video() const {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "get_video";
  return video;
}

void participant_model::set_name() {
  auto name_standard = participant_.get_name();
  name = QString::fromStdString(name_standard);
  name_changed(name);
}

void participant_model::video_added(rtc::google::video_source &added) {
  BOOST_ASSERT(!video); // TODO support more than one video per client
  video = new ui::frame_provider_google_video_source(this);
  video->set_source(&added);
  video_changed(video);
}
