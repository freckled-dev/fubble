#include "participant_model.hpp"
#include "client/audio_device_settings.hpp"
#include "client/audio_level_calculator.hpp"
#include "client/audio_tracks_volume.hpp"
#include "client/own_audio_information.hpp"
#include "client/own_participant.hpp"
#include "client/participant.hpp"
#include "client/video_settings.hpp"
#include "rtc/google/audio_track.hpp"

using namespace client;

participant_model::participant_model(participant &participant_,
                                     audio_device_settings &audio_settings_,
                                     video_settings &video_settings_,
                                     own_audio_information &audio_information_,
                                     audio_volume &audio_volume_,
                                     QObject *parent)
    : QObject(parent), participant_(participant_),
      audio_settings_(audio_settings_), video_settings_(video_settings_),
      audio_information_(audio_information_), audio_volume_(audio_volume_),
      id(participant_.get_id()), own{dynamic_cast<own_participant *>(
                                         &participant_) != nullptr} {
  set_name();
  signal_connections.push_back(
      participant_.on_name_changed.connect([this](auto) { set_name(); }));
  signal_connections.push_back(participant_.on_video_added.connect(
      [this](auto &added) { video_added(added); }));
  signal_connections.push_back(participant_.on_video_removed.connect(
      [this](auto &removed) { video_removed(removed); }));
  auto videos = participant_.get_videos();
  for (auto video : videos) {
    BOOST_ASSERT(video);
    video_added(*video);
  }
  if (own) {
    video_disabled = video_settings_.get_paused();
    signal_connections.push_back(
        audio_information_.on_sound_level_30times_a_second.connect(
            [this](auto level) { on_sound_level(level); }));
    signal_connections.push_back(audio_information_.on_voice_detected.connect(
        [this](auto detected) { on_voice_detected(detected); }));
  } else {
    signal_connections.push_back(participant_.on_audio_added.connect(
        [this](auto &source) { audio_added(source.get_source()); }));
    signal_connections.push_back(participant_.on_audio_removed.connect(
        [this](auto &source) { audio_removed(source.get_source()); }));
    auto audios = participant_.get_audios();
    for (auto audio : audios) {
      BOOST_ASSERT(audio);
      audio_added(audio->get_source());
    }
    volume = audio_volume_.get_volume(id);
    muted = audio_volume_.get_muted(id);
  }
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
  // only gets called if !own
  BOOST_ASSERT(
      !audio_level_calculator_); // TODO support more than one audio source per
                                 // client. although, does it make sense?!
  audio_level_calculator_ = std::make_unique<audio_level_calculator>(source);
  audio_level_calculator_->on_sound_level_30times_a_second.connect(
      [this](auto level) { on_sound_level(level); });
  audio_level_calculator_->on_voice_detected.connect(
      [this](auto detected) { on_voice_detected(detected); });
}

void participant_model::audio_removed(rtc::google::audio_source &remove) {
  if (&audio_level_calculator_->get_source() != &remove)
    return;
  return audio_level_calculator_.reset();
}

void participant_model::on_sound_level(double level) {
#if 0
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
#endif
  int audio_level = std::min(127, static_cast<int>(level * 127.0));
  newAudioLevel(audio_level);
}

void participant_model::on_voice_detected(bool detected) {
  voice_detected = detected;
  voice_detected_changed(voice_detected);
}

qreal participant_model::get_volume() const {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  return volume;
}

void participant_model::set_volume(qreal volume_) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", volume:" << volume;
  audio_volume_.set_volume(id, volume_);
  volume = volume_;
  volume_changed(volume_);
}

bool participant_model::get_silenced() const { return silenced; }

void participant_model::set_silenced(bool change) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", change:" << change;
  if (change == silenced)
    return;
  silenced = change;
  silenced_changed(silenced);
}
