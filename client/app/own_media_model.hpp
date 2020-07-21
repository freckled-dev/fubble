#ifndef UUID_04C6146E_9EA8_4A08_B962_D5AEEEEC9087
#define UUID_04C6146E_9EA8_4A08_B962_D5AEEEEC9087

#include "client/logger.hpp"
#include <QObject>

namespace client {
class own_media;
class video_settings;
class own_audio_information;
class own_media_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(
      bool videoDisabled MEMBER video_disabled NOTIFY video_disabled_changed)
  Q_PROPERTY(bool muted MEMBER muted NOTIFY muted_changed)
public:
  own_media_model(video_settings &video_settings_, own_media &own_media_,
                  own_audio_information &audio_information_);
  ~own_media_model();

signals:
  void video_disabled_changed(bool);
  void muted_changed(bool);
  void newAudioLevel(int level);

protected:
  void change_muted(bool);
  void change_video_diabled(bool);

  client::logger logger{"own_media_model"};
  void on_sound_level(const double);

  own_media &own_media_;
  own_audio_information &audio_information_;
  video_settings &video_settings_;
  bool video_disabled{true};
  int audio_level{};
  bool muted{false};
};
} // namespace client

#endif
