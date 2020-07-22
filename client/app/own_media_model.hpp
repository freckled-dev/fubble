#ifndef UUID_04C6146E_9EA8_4A08_B962_D5AEEEEC9087
#define UUID_04C6146E_9EA8_4A08_B962_D5AEEEEC9087

#include "client/logger.hpp"
#include <QObject>

namespace client {
namespace ui {
class frame_provider_google_video_source;
}
class audio_settings;
class loopback_audio;
class own_audio_information;
class own_media;
class video_settings;
class own_media_model : public QObject {
  Q_OBJECT
  // for join screen
  Q_PROPERTY(client::ui::frame_provider_google_video_source *video MEMBER video
                 NOTIFY video_changed)
  // for settings
  Q_PROPERTY(client::ui::frame_provider_google_video_source *videoPreview MEMBER
                 video_preview NOTIFY video_preview_changed)
  Q_PROPERTY(bool videoDisabled MEMBER video_disabled WRITE set_video_disabled
                 NOTIFY video_disabled_changed)
  Q_PROPERTY(bool videoAvailable READ get_video_available NOTIFY
                 video_available_changed)
  Q_PROPERTY(bool muted MEMBER muted WRITE set_muted NOTIFY muted_changed)
  Q_PROPERTY(bool deafed MEMBER deafed WRITE set_deafed NOTIFY deafed_changed)
  Q_PROPERTY(bool loopbackOwnVoice READ get_loopback_audio WRITE
                 set_loopback_audio NOTIFY loopback_audio_changed)

public:
  own_media_model(audio_settings &audio_settings_,
                  video_settings &video_settings_,
                  loopback_audio &loopback_audio_,
                  own_audio_information &audio_information_,
                  own_media &own_media_);
  ~own_media_model();

signals:
  void video_disabled_changed(bool);
  void muted_changed(bool);
  void newAudioLevel(int level);
  void deafed_changed(bool);
  void video_available_changed(bool);
  void video_changed(ui::frame_provider_google_video_source *);
  void video_preview_changed(ui::frame_provider_google_video_source *);
  void loopback_audio_changed(bool);

protected:
  void set_muted(bool);
  void set_deafed(bool);
  void set_video_disabled(bool);
  bool get_video_available() const;
  void on_sound_level(const double);
  void update_video();
  void set_loopback_audio(bool);
  bool get_loopback_audio() const;

  client::logger logger{"own_media_model"};
  own_audio_information &audio_information_;
  video_settings &video_settings_;
  audio_settings &audio_settings_;
  loopback_audio &loopback_audio_;
  own_media &own_media_;
  bool video_disabled{};
  int audio_level{};
  bool muted{};
  bool deafed{};
  ui::frame_provider_google_video_source *video{};
  ui::frame_provider_google_video_source *video_preview{};
};
} // namespace client

#endif
