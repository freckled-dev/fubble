#include "participant_model.hpp"
#include "client/audio_settings.hpp"
#include "client/participant.hpp"

using namespace client;

participant_model::participant_model(participant &participant_,
                                     audio_settings &audio_settings_,
                                     QObject *parent)
    : QObject(parent), participant_(participant_),
      audio_settings_(audio_settings_), id(participant_.get_id()) {
  set_name();
  participant_.on_name_changed.connect([this](auto) { set_name(); });
  // TODO support video removal
  participant_.on_video_added.connect([this](auto added) {
    BOOST_ASSERT(added);
    video_added(*added);
  });
  auto videos = participant_.get_videos();
  for (auto video : videos) {
    BOOST_ASSERT(video);
    video_added(*video);
  }
  connect(this, &participant_model::muted_changed, this,
          &participant_model::on_muted_changed);
  connect(this, &participant_model::deafed_changed, this,
          &participant_model::on_deafed_changed);
}

std::string participant_model::get_id() const { return id; }

ui::frame_provider_google_video_source *participant_model::get_video() const {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "get_video";
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

void participant_model::on_muted_changed(bool muted_) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "muted_changed, muted_:" << muted_;
  audio_settings_.mute_microphone(muted || deafed);
}

void participant_model::on_deafed_changed(bool deafed_) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "deafed_changed, deafed_:" << deafed_;
  audio_settings_.mute_microphone(muted || deafed);
  audio_settings_.mute_speaker(deafed);
}
