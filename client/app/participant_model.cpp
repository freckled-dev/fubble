#include "participant_model.hpp"
#include "client/audio_settings.hpp"
#include "client/participant.hpp"

using namespace client;

participant_model::participant_model(participant &participant_,
                                     audio_settings &audio_settings_,
                                     QObject *parent)
    : QObject(parent), participant_(participant_),
      audio_settings_(audio_settings_), id(participant_.get_id()),
      identifier(QString::fromStdString(id)) {
  set_name();
  participant_.on_name_changed.connect([this](auto) { set_name(); });
  // TODO support video removal
  participant_.on_video_added.connect([this](auto added) {
    BOOST_ASSERT(added);
    video_added(*added);
  });
  participant_.on_sound_level.connect(
      [this](auto level) { on_sound_level(level); });
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

void participant_model::on_sound_level(double level) {
  ++audio_level_counter;
  if (audio_level_counter > audio_level_values_to_collect) {
    audio_level = static_cast<int>(audio_level_cache *= 127.0);
    audio_level = std::min(127, audio_level);
    audio_level_changed(audio_level);
#if 1
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "on_sound_level, level:" << audio_level;
#endif
    audio_level_counter = 0;
    audio_level_cache = 0.;
  }
  audio_level_cache +=
      level / static_cast<double>(audio_level_values_to_collect);

  ++voice_detected_counter;
  if (voice_detected_counter > voice_audio_level_values_to_collect) {
    const bool voice_detected_current =
        voice_detected_audio_level_cache > voice_detected_threshold;
#if 1
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "voice_detected, voice_detected_current:" << voice_detected_current
        << ", voice_detected_audio_level_cache:"
        << voice_detected_audio_level_cache;
#endif
    if (voice_detected_current != voice_detected) {
      voice_detected = voice_detected_current;
      voice_detected_changed(voice_detected);
    }
    voice_detected_audio_level_cache = 0.;
    voice_detected_counter = 0;
  }
  voice_detected_audio_level_cache +=
      level / static_cast<double>(voice_audio_level_values_to_collect);
}
