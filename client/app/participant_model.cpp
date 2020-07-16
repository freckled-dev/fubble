#include "participant_model.hpp"
#include "client/audio_level_calculator.hpp"
#include "client/audio_settings.hpp"
#include "client/own_audio_information.hpp"
#include "client/own_participant.hpp"
#include "client/participant.hpp"

using namespace client;

participant_model::participant_model(participant &participant_,
                                     audio_settings &audio_settings_,
                                     own_audio_information &audio_information_,
                                     QObject *parent)
    : QObject(parent), participant_(participant_),
      audio_settings_(audio_settings_), audio_information_(audio_information_),
      id(participant_.get_id()), own{dynamic_cast<own_participant *>(
                                         &participant_) != nullptr} {
  set_name();
  participant_.on_name_changed.connect([this](auto) { set_name(); });
  participant_.on_video_added.connect(
      [this](auto &added) { video_added(added); });
  participant_.on_video_removed.connect(
      [this](auto &removed) { video_removed(removed); });
  auto videos = participant_.get_videos();
  for (auto video : videos) {
    BOOST_ASSERT(video);
    video_added(*video);
  }
  if (own) {
    audio_information_.on_sound_level_30times_a_second.connect(
        [this](auto level) { on_sound_level(level); });
    audio_information_.on_voice_detected.connect(
        [this](auto detected) { on_voice_detected(detected); });
  } else {
    // TODO support audio removal!
    participant_.on_audio_added.connect(
        [this](auto &source) { audio_added(source); });
    auto audios = participant_.get_audios();
    for (auto audio : audios) {
      BOOST_ASSERT(audio);
      audio_added(*audio);
    }
  }
  connect(this, &participant_model::muted_changed, this,
          &participant_model::on_muted_changed);
  connect(this, &participant_model::deafed_changed, this,
          &participant_model::on_deafed_changed);
}

participant_model::~participant_model() = default;

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
  // TODO support more than one video per client
  if (video) {
    BOOST_LOG_SEV(logger, logging::severity::warning) << "replacing a video!";
    video->deleteLater();
  }
  video = new ui::frame_provider_google_video_source(this);
  video->set_source(&added);
  video_changed(video);
}

void participant_model::video_removed(rtc::google::video_source &removed) {
  BOOST_ASSERT(video);
  if (video->get_source() != &removed) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << "can't remove video, because it seems like it got replaced.";
    return;
  }
  video->deleteLater();
  video = nullptr;
  video_changed(video);
}

void participant_model::audio_added(rtc::google::audio_source &source) {
  BOOST_ASSERT(
      !audio_level_calculator_); // TODO support more than one audio source per
                                 // client. although, does it make sense?!
  audio_level_calculator_ = std::make_unique<audio_level_calculator>(source);
  audio_level_calculator_->on_sound_level_30times_a_second.connect(
      [this](auto level) { on_sound_level(level); });
  audio_level_calculator_->on_voice_detected.connect(
      [this](auto detected) { on_voice_detected(detected); });
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
#if 0
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
#endif
  audio_level = std::min(127, static_cast<int>(level * 127.0));
  audio_level_changed(audio_level);
}

void participant_model::on_voice_detected(bool detected) {
  voice_detected = detected;
  voice_detected_changed(voice_detected);
}
