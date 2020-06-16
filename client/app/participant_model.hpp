#ifndef UUID_912B7A62_19DE_45A9_B92E_0FC0046CE8C5
#define UUID_912B7A62_19DE_45A9_B92E_0FC0046CE8C5

#include "client/logger.hpp"
#include "rtc/google/video_source_ptr.hpp"
#include "ui/frame_provider_google_video_frame.hpp"
#include <QObject>

namespace client {
class participant;
class audio_settings;

class participant_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString name MEMBER name NOTIFY name_changed)
  Q_PROPERTY(bool own MEMBER own NOTIFY own_changed)
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
  Q_PROPERTY(int audioLevel MEMBER audio_level NOTIFY audio_level_changed)

public:
  participant_model(participant &participant_, audio_settings &audio_settings_,
                    QObject *parent);

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
  void audio_level_changed(int);

protected:
  void set_name();
  void video_added(rtc::google::video_source &);
  void on_muted_changed(bool muted_);
  void on_deafed_changed(bool muted_);
  void on_sound_level(double level);

  mutable client::logger logger{"participant_model"};
  participant &participant_;
  audio_settings &audio_settings_;
  const std::string id;
  QString name;
  bool own{};
  bool muted{};
  bool deafed{};
  bool silenced{};
  bool volume = 1;
  bool video_disabled{};
  bool highlighted{};
  ui::frame_provider_google_video_source *video{};
  bool voice_detected{};
  int audio_level{};

  double audio_level_cache{};
  int audio_level_counter{};
};

} // namespace client

// Q_DECLARE_METATYPE(client::participant_model *)

#endif
