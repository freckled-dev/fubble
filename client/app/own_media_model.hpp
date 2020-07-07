#ifndef UUID_04C6146E_9EA8_4A08_B962_D5AEEEEC9087
#define UUID_04C6146E_9EA8_4A08_B962_D5AEEEEC9087

#include <QObject>

namespace client {
class own_media;
class own_audio_information;
class own_media_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(
      bool videoDisabled MEMBER video_disabled NOTIFY video_disabled_changed)
  Q_PROPERTY(bool audioLevel MEMBER audio_level NOTIFY audio_level_changed)
  Q_PROPERTY(bool muted MEMBER muted NOTIFY muted_changed)
public:
  own_media_model(own_media &own_media_,
                  own_audio_information &audio_information_);
  ~own_media_model();

signals:
  void video_disabled_changed(bool);
  void audio_level_changed(bool);
  void muted_changed(bool);

protected:
  own_media &own_media_;
  own_audio_information &audio_information_;
  bool video_disabled{true};
  int audio_level{};
  bool muted{false};
};
} // namespace client

#endif