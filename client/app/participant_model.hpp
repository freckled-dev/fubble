#ifndef UUID_912B7A62_19DE_45A9_B92E_0FC0046CE8C5
#define UUID_912B7A62_19DE_45A9_B92E_0FC0046CE8C5

#include "client/logger.hpp"
#include "rtc/google/audio_source_ptr.hpp"
#include "rtc/google/video_source_ptr.hpp"
#include "ui/frame_provider_google_video_frame.hpp"
#include <QObject>

namespace client {
class participant;
class audio_settings;
class audio_level_calculator;
class own_audio_information;
class video_settings;

// TODO derive class to `own_participant_model` and `remote_participant_model`
class participant_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString identifier MEMBER identifier CONSTANT);
  Q_PROPERTY(QString name MEMBER name NOTIFY name_changed)
  Q_PROPERTY(bool own MEMBER own CONSTANT)
  // self muted
  Q_PROPERTY(bool muted MEMBER muted NOTIFY muted_changed)
  // all others muted
  Q_PROPERTY(bool deafed MEMBER deafed NOTIFY deafed_changed)
  // specific participant muted
  Q_PROPERTY(bool silenced MEMBER silenced NOTIFY silenced_changed)
  // volume setting from 0 to 1
  Q_PROPERTY(double volume MEMBER volume NOTIFY volume_changed)
  // own video disabled
  Q_PROPERTY(
      bool videoDisabled MEMBER video_disabled NOTIFY video_disabled_changed)
  // just used in the GUI - do not change it
  Q_PROPERTY(bool highlighted MEMBER highlighted NOTIFY highlighted_changed)

  Q_PROPERTY(client::ui::frame_provider_google_video_source *video READ
                 get_video NOTIFY video_changed)
  Q_PROPERTY(
      bool voiceDetected MEMBER voice_detected NOTIFY voice_detected_changed)

public:
  participant_model(participant &participant_, audio_settings &audio_settings_,
                    video_settings &video_settings_,
                    own_audio_information &audio_information_, QObject *parent);
  ~participant_model();

  std::string get_id() const;
  ui::frame_provider_google_video_source *get_video() const;

signals:
  void name_changed(QString);
  void own_changed(bool);
  void muted_changed(bool);
  void deafed_changed(bool);
  void silenced_changed(bool);
  void volume_changed(double);
  void video_disabled_changed(bool);
  void highlighted_changed(bool);
  void video_changed(ui::frame_provider_google_video_source *);
  void voice_detected_changed(bool);
  void newAudioLevel(int level);

protected:
  void set_name();
  void video_added(rtc::google::video_source &);
  void video_removed(rtc::google::video_source &);
  // TODO refactor to track
  void audio_added(rtc::google::audio_source &);
  void on_sound_level(double level);
  void on_voice_detected(bool detected);

  mutable client::logger logger{"participant_model"};
  participant &participant_;
  audio_settings &audio_settings_;
  video_settings &video_settings_;
  own_audio_information &audio_information_;
  std::unique_ptr<audio_level_calculator> audio_level_calculator_;
  const std::string id;
  const QString identifier{QString::fromStdString(id)};
  QString name;
  const bool own{};
  bool muted{};
  bool deafed{};
  bool silenced{};
  double volume{1.};
  bool video_disabled{};
  bool highlighted{};
  ui::frame_provider_google_video_source *video{};
  bool voice_detected{};
  int audio_level{};
};

} // namespace client

// Q_DECLARE_METATYPE(client::participant_model *)

#endif
