#ifndef UUID_44723430_4624_4131_9BF4_5C1475643AB0
#define UUID_44723430_4624_4131_9BF4_5C1475643AB0

#include "logging/logger.hpp"
#include "rtc/google/video_source.hpp"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#include <QtMultimedia/qabstractvideosurface.h>
#include <QtMultimedia/qvideosurfaceformat.h>
#pragma GCC diagnostic pop

namespace client::ui {
// https://stackoverflow.com/questions/43854589/custom-source-property-for-videooutput-qml
// https://doc.qt.io/qt-5/videooverview.html
// TODO name the source file the same way the class is called
class frame_provider_google_video_source : public QObject {
  Q_OBJECT
  Q_PROPERTY(
      QAbstractVideoSurface *videoSurface READ get_surface WRITE set_surface)
public:
  frame_provider_google_video_source();
  ~frame_provider_google_video_source();

  void set_source(rtc::google::video_source *source);
  void set_surface(QAbstractVideoSurface *surface);
  QAbstractVideoSurface *get_surface() const;

protected:
  void on_frame(const webrtc::VideoFrame &frame);
  void on_frame_ui_thread(const QVideoFrame &frame);
  void set_size(const QSize &set);

  logging::logger logger;
  QAbstractVideoSurface *surface{};
  rtc::google::video_source *source{};
  QVideoSurfaceFormat format;
  QSize current_frame_size;
  boost::signals2::scoped_connection connection_on_frame;
};
} // namespace client::ui
// Q_DECLARE_METATYPE(client::ui::frame_provider_google_video_source)
// Q_DECLARE_METATYPE(client::ui::frame_provider_google_video_source *)

#endif
