#include "own_media_model.hpp"
#include "client/audio_settings.hpp"
#include "client/own_audio_information.hpp"
#include "client/own_media.hpp"
#include "client/ui/frame_provider_google_video_frame.hpp"
#include "client/video_settings.hpp"

using namespace client;

own_media_model::own_media_model(audio_settings &audio_settings_,
                                 video_settings &video_settings_,
                                 own_audio_information &audio_information_,
                                 own_media &own_media_)
    : audio_information_(audio_information_), video_settings_(video_settings_),
      audio_settings_(audio_settings_), own_media_(own_media_) {
  audio_information_.on_sound_level_30times_a_second.connect(
      [this](auto level) { on_sound_level(level); });
  update_video();
  // video_settings_.on_video_source_changed; // cleaner?
  own_media_.on_video_added.connect([this](auto &) { update_video(); });
  own_media_.on_video_removed.connect([this](auto &) { update_video(); });
}

own_media_model::~own_media_model() = default;

void own_media_model::set_muted(bool muted_) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", muted:" << muted_;
  muted = muted_;
  audio_settings_.mute_microphone(muted || deafed);
}

void own_media_model::set_deafed(bool deafed_) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", deafed:" << deafed_;
  deafed = deafed_;
  audio_settings_.mute_microphone(muted || deafed);
  audio_settings_.mute_speaker(deafed);
}

void own_media_model::set_video_disabled(bool disabled) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", disabled:" << disabled;
  video_disabled = disabled;
  video_settings_.pause(disabled);
}

bool own_media_model::get_video_available() const {
  return video_settings_.is_a_video_available();
}

void own_media_model::on_sound_level(const double level) {
  int audio_level = std::min(127, static_cast<int>(level * 127.0));
  newAudioLevel(audio_level);
}

void own_media_model::update_video() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  if (video != nullptr) {
    BOOST_ASSERT(video_preview != nullptr);
    video->deleteLater();
    video = nullptr;
    video_changed(video);
    video_preview->deleteLater();
    video_preview = nullptr;
    video_changed(video_preview);
  }

  auto own_videos = own_media_.get_videos();
  if (own_videos.empty()) {
    return;
  }

  BOOST_ASSERT(own_videos.size() == 1);
  auto own_video = own_videos.front();
  video = new ui::frame_provider_google_video_source(this);
  video->set_source(own_video);
  video_changed(video);
  video_preview = new ui::frame_provider_google_video_source(this);
  video_preview->set_source(own_video);
  video_changed(video_preview);
}
