#ifndef UUID_44723430_4624_4131_9BF4_5C1475643AB0
#define UUID_44723430_4624_4131_9BF4_5C1475643AB0

#include "fubble/client/logger.hpp"
#include "fubble/rtc/google/video_source.hpp"
#include <QtMultimedia/qabstractvideosurface.h>
#include <QtMultimedia/qvideosurfaceformat.h>

namespace client::ui {
// https://stackoverflow.com/questions/43854589/custom-source-property-for-videooutput-qml
// https://doc.qt.io/qt-5/videooverview.html
// TODO name the source file the same way the class is called
class frame_provider_google_video_source : public QObject {
  Q_OBJECT
  Q_PROPERTY(
      QAbstractVideoSurface *videoSurface READ get_surface WRITE set_surface)
public:
  frame_provider_google_video_source(QObject *parent);
  ~frame_provider_google_video_source();

  // TODO take it in constructor?! reinstance
  // `frame_provider_google_video_source` every video change
  void set_source(std::shared_ptr<rtc::google::video_source> source);
  std::shared_ptr<rtc::google::video_source> get_source() const;
  void set_surface(QAbstractVideoSurface *surface);
  QAbstractVideoSurface *get_surface() const;

protected:
  void on_frame(const webrtc::VideoFrame &frame);
  void on_frame_ui_thread(const QVideoFrame &frame);
  void set_format(const QSize &set);
  void start_surface();

  client::logger logger{"frame_provider_google_video_source"};
  QAbstractVideoSurface *surface{};
  std::shared_ptr<rtc::google::video_source> source;
  QVideoSurfaceFormat format;
  QSize current_frame_size;
  boost::signals2::scoped_connection connection_on_frame;
};
} // namespace client::ui

#endif
