#include "own_media_model.hpp"
#include "client/audio_device_settings.hpp"
#include "client/audio_tracks_volume.hpp"
#include "client/own_audio_information.hpp"
#include "client/own_media.hpp"
#include "client/own_microphone_tester.hpp"
#include "client/ui/frame_provider_google_video_frame.hpp"
#include "client/video_settings.hpp"

using namespace client;

own_media_model::own_media_model(audio_device_settings &audio_settings_,
                                 video_settings &video_settings_,
                                 own_microphone_tester &audio_tester,
                                 audio_tracks_volume &audio_tracks_volume_,
                                 own_audio_information &audio_information_,
                                 own_audio_information &audio_test_information_,
                                 own_media &own_media_)
    : audio_settings_(audio_settings_), video_settings_(video_settings_),
      audio_tester(audio_tester), audio_tracks_volume_(audio_tracks_volume_),
      audio_information_(audio_information_), own_media_(own_media_) {
  muted = audio_tracks_volume_.get_self_muted();
  deafed = audio_tracks_volume_.get_deafen();
  signal_connections.push_back(
      audio_information_.on_sound_level_30times_a_second.connect(
          [this](auto level) { on_sound_level(level); }));
  signal_connections.push_back(
      audio_test_information_.on_sound_level_30times_a_second.connect(
          [this](auto level) { on_sound_test_level(level); }));
  update_video();
  signal_connections.push_back(video_settings_.on_video_source_changed.connect(
      [this]() { update_video(); }));
}

own_media_model::~own_media_model() = default;

void own_media_model::set_muted(bool muted_) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", muted:" << muted_;
  muted = muted_;
  audio_tracks_volume_.mute_self(muted);
  muted_changed(muted);
}

void own_media_model::set_deafed(bool deafed_) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", deafed:" << deafed_;
  deafed = deafed_;
  audio_tracks_volume_.deafen(deafed);
  deafed_changed(deafed_);
}

void own_media_model::set_video_disabled(bool disabled) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", disabled:" << disabled;
  video_disabled = disabled;
  video_disabled_changed(disabled);
  video_settings_.pause(disabled);
  video_disabled_changed(disabled);
}

bool own_media_model::get_video_available() const {
  return video_settings_.is_a_video_available();
}

void own_media_model::on_sound_level(const double level) {
  int audio_level = std::min(127, static_cast<int>(level * 127.0));
  newAudioLevel(audio_level);
}

void own_media_model::on_sound_test_level(const double level) {
  int audio_level = std::min(127, static_cast<int>(level * 127.0));
  newAudioTestLevel(audio_level);
}

void own_media_model::update_video() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  if (video != nullptr) {
    video->deleteLater();
    video = nullptr;
    video_changed(video);
  }

  auto own_videos = own_media_.get_videos().get_all();
  if (own_videos.empty()) {
    video_available_changed(get_video_available());
    return;
  }

  BOOST_ASSERT(own_videos.size() == 1);
  auto own_video = own_videos.front();
  video = new ui::frame_provider_google_video_source(this);
  video->set_source(own_video);
  video_changed(video);
  video_available_changed(get_video_available());
}

void own_media_model::set_loopback_audio(bool change) {
  if (change == audio_tester.get_started())
    return;
  if (change)
    audio_tester.start();
  else
    audio_tester.stop();
  loopback_audio_changed(change);
}

bool own_media_model::get_loopback_audio() const {
  bool result = audio_tester.get_started();
  return result;
}
