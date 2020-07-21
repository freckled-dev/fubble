#include "own_media_model.hpp"
#include "client/own_audio_information.hpp"
#include "client/video_settings.hpp"

using namespace client;

own_media_model::own_media_model(video_settings &video_settings_,
                                 own_media &own_media_,
                                 own_audio_information &audio_information_)
    : own_media_(own_media_), audio_information_(audio_information_),
      video_settings_(video_settings_) {
  connect(this, &own_media_model::muted_changed, this,
          &own_media_model::change_muted);
  connect(this, &own_media_model::video_disabled_changed, this,
          &own_media_model::change_video_diabled);
}

own_media_model::~own_media_model() = default;

void own_media_model::change_muted(bool muted_) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", muted:" << muted_;
}

void own_media_model::change_video_diabled(bool disabled) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", disabled:" << disabled;
  video_settings_.pause(disabled);
}

