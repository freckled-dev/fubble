#ifndef UUID_85C81A68_0C92_497B_8018_52B904D1012F
#define UUID_85C81A68_0C92_497B_8018_52B904D1012F

#include "client/logger.hpp"
#include <QObject>

class QAbstractVideoSurface;
namespace rtc::google::capture::video {
class device;
}
namespace client::ui {
class frame_provider_google_video_source;
class frame_provider_google_video_device : public QObject {
  Q_OBJECT
  Q_PROPERTY(
      QAbstractVideoSurface *videoSurface READ get_surface WRITE set_surface)
public:
  frame_provider_google_video_device(
      rtc::google::capture::video::device &device, QObject *parent);
  ~frame_provider_google_video_device();

  void set_surface(QAbstractVideoSurface *surface);
  QAbstractVideoSurface *get_surface() const;

  Q_INVOKABLE void play();
  Q_INVOKABLE void stop();
  bool get_playing() const;

protected:
  rtc::google::capture::video::device &device;
  frame_provider_google_video_source *delegate{};
};
} // namespace client::ui

#endif
