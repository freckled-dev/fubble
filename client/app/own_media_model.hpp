#ifndef UUID_04C6146E_9EA8_4A08_B962_D5AEEEEC9087
#define UUID_04C6146E_9EA8_4A08_B962_D5AEEEEC9087

#include "client/logger.hpp"
#include <QObject>

namespace client {
class loopback_audio;
class own_audio_information;
class own_media;
class video_settings;
class own_media_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(
      bool videoDisabled MEMBER video_disabled NOTIFY video_disabled_changed)
  Q_PROPERTY(bool muted MEMBER muted NOTIFY muted_changed)
  Q_PROPERTY(bool deafed MEMBER deafed NOTIFY deafed_changed)
  Q_PROPERTY(bool loopbackOwnVoice READ get_loopback_audio WRITE
                 set_loopback_audio NOTIFY loopback_audio_changed)
public:
  own_media_model(video_settings &video_settings_, own_media &own_media_,
                  loopback_audio &loopback_audio_,
                  own_audio_information &audio_information_);
  ~own_media_model();

signals:
  void video_disabled_changed(bool);
  void muted_changed(bool);
  void newAudioLevel(int level);
  void deafed_changed(bool);
  void loopback_audio_changed(bool);

protected:
  void change_muted(bool);
  void change_video_diabled(bool);
  void on_sound_level(const double);
  void set_loopback_audio(bool);
  bool get_loopback_audio() const;

  mutable client::logger logger{"own_media_model"};
  video_settings &video_settings_;
  own_media &own_media_;
  loopback_audio &loopback_audio_;
  own_audio_information &audio_information_;
  bool video_disabled{};
  int audio_level{};
  bool muted{};
  bool deafed{};
};
} // namespace client

#endif
