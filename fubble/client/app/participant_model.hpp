#ifndef UUID_912B7A62_19DE_45A9_B92E_0FC0046CE8C5
#define UUID_912B7A62_19DE_45A9_B92E_0FC0046CE8C5

#include "fubble/client/logger.hpp"
#include "fubble/rtc/google/audio_source_ptr.hpp"
#include "fubble/rtc/google/video_source_ptr.hpp"
#include "ui/frame_provider_google_video_frame.hpp"
#include <QObject>

namespace client {
class participant;
class audio_device_settings;
class audio_level_calculator;
class audio_level_calculator_factory;
class own_audio_information;
class audio_volume;
class video_settings;
class mute_deaf_communicator;

// TODO derive class to `own_participant_model` and `remote_participant_model`
class participant_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString identifier MEMBER identifier CONSTANT)
  Q_PROPERTY(QString name MEMBER name NOTIFY name_changed)
  Q_PROPERTY(bool own MEMBER own CONSTANT)
  // self muted
  Q_PROPERTY(bool muted MEMBER muted NOTIFY muted_changed)
  // all others muted
  Q_PROPERTY(bool deafed MEMBER deafed NOTIFY deafed_changed)
  // specific participant muted
  Q_PROPERTY(bool silenced READ get_silenced WRITE set_silenced NOTIFY
                 silenced_changed)
  // volume setting from 0 to 1
  Q_PROPERTY(
      qreal volume READ get_volume WRITE set_volume NOTIFY volume_changed)
  // just used in the GUI - do not change it
  Q_PROPERTY(bool highlighted MEMBER highlighted NOTIFY highlighted_changed)

  Q_PROPERTY(client::ui::frame_provider_google_video_source *video READ
                 get_video NOTIFY video_changed)
  Q_PROPERTY(
      bool voiceDetected MEMBER voice_detected NOTIFY voice_detected_changed)

public:
  participant_model(
      audio_level_calculator_factory &audio_level_calculator_factory_,
      participant &participant_, audio_device_settings &audio_settings_,
      video_settings &video_settings_,
      own_audio_information &audio_information_, audio_volume &audio_volume_,
      std::shared_ptr<mute_deaf_communicator> muted_deaf_communicator_,
      QObject *parent);
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
  void video_added(std::shared_ptr<rtc::google::video_source>);
  void video_removed(std::shared_ptr<rtc::google::video_source>);
  // TODO refactor to track
  void audio_added(rtc::google::audio_source &);
  void audio_removed(rtc::google::audio_source &);
  void on_sound_level(double level);
  void on_voice_detected(bool detected);
  qreal get_volume() const;
  void set_volume(qreal);
  bool get_silenced() const;
  void set_silenced(bool);
  void on_muted(const std::string &id, const bool muted);
  void on_deafed(const std::string &id, const bool deafed);

  mutable client::logger logger{"participant_model"};
  audio_level_calculator_factory &audio_level_calculator_factory_;
  participant &participant_;
  audio_device_settings &audio_settings_;
  video_settings &video_settings_;
  own_audio_information &audio_information_;
  audio_volume &audio_volume_;
  std::shared_ptr<mute_deaf_communicator> muted_deaf_communicator_;
  std::unique_ptr<audio_level_calculator> audio_level_calculator_;
  const std::string id;
  const QString identifier{QString::fromStdString(id)};
  QString name;
  const bool own{};
  bool muted{};
  bool deafed{};
  bool silenced{};
  double volume{1.};
  bool highlighted{};
  ui::frame_provider_google_video_source *video{};
  bool voice_detected{};
  int audio_level{};
  std::vector<boost::signals2::scoped_connection> signal_connections;
};

} // namespace client

// Q_DECLARE_METATYPE(client::participant_model *)

#endif
