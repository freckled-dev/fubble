#include "logging/initialser.hpp"
#include "logging/logger.hpp"
#include "rtc/google/capture/video/device.hpp"
#include "rtc/google/capture/video/device_creator.hpp"
#include "rtc/google/capture/video/enumerator.hpp"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtMultimedia/qabstractvideosurface.h>
#include <QtMultimedia/qvideosurfaceformat.h>
#pragma GCC diagnostic pop

class MyVideoProducer : public QObject {
  Q_OBJECT
  Q_PROPERTY(QAbstractVideoSurface *videoSurface READ videoSurface WRITE
                 setVideoSurface)

public:
  QAbstractVideoSurface *videoSurface() const { return m_surface; }

  void setVideoSurface(QAbstractVideoSurface *surface) {
    if (m_surface != surface && m_surface && m_surface->isActive()) {
      m_surface->stop();
    }
    m_surface = surface;
    if (m_surface)
      m_surface->start(m_format);
  }
public slots:
  void onNewVideoContentReceived(const QVideoFrame &frame) {
    if (m_surface)
      m_surface->present(frame);
  }

private:
  QAbstractVideoSurface *m_surface;
  QVideoSurfaceFormat m_format;
};

int main(int argc, char *argv[]) {
  logging::add_console_log();
  logging::logger logger;
  BOOST_LOG_SEV(logger, logging::severity::debug) << "starting up";

  rtc::google::capture::video::enumerator enumerator;
  auto devices = enumerator();
  for (const auto device : devices)
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "capture device:" << device.name;
  if (devices.empty())
    throw std::runtime_error("no video capture devices available");
  rtc::google::capture::video::device_creator device_creator;
  std::shared_ptr<rtc::google::capture::video::device> capture_device =
      device_creator(devices.front().id);
  capture_device->on_frame.connect([&](const auto &frame) {
    (void)frame;
    BOOST_LOG_SEV(logger, logging::severity::debug) << "frame";
  });
  capture_device->start();

  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication app(argc, argv);

  QQmlApplicationEngine engine;
  const QUrl url(QStringLiteral("qrc:/main.qml"));
  engine.load(url);

  // TODO deliver frames to qml
  // https://stackoverflow.com/questions/43854589/custom-source-property-for-videooutput-qml
  // https://doc.qt.io/qt-5/videooverview.html

  return app.exec();
}
