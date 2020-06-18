#include <QObject>

namespace client {
class own_media_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(
      bool videoEnabled MEMBER video_enabled NOTIFY video_enabled_changed)
  Q_PROPERTY(bool audioLevel MEMBER audio_level NOTIFY audio_level_changed)
  Q_PROPERTY(bool muted MEMBER muted NOTIFY muted_changed)
public:
  own_media_model();
  ~own_media_model();

signals:
  void video_enabled_changed(bool);
  void audio_level_changed(bool);
  void muted_changed(bool);

protected:
  bool video_enabled{true};
  int audio_level{};
  bool muted{false};
};
} // namespace client
