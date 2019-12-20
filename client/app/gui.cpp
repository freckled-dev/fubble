#include "logging/initialser.hpp"
#include "logging/logger.hpp"
#include "rtc/google/capture/video/device.hpp"
#include "rtc/google/capture/video/device_creator.hpp"
#include "rtc/google/capture/video/enumerator.hpp"
#include "ui/frame_provider_google_video_frame.hpp"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#pragma GCC diagnostic pop

int main(int argc, char *argv[]) {
  logging::add_console_log();
  logging::logger logger;
  BOOST_LOG_SEV(logger, logging::severity::debug) << "starting up";

  rtc::google::capture::video::enumerator enumerator;
  auto devices = enumerator();
  for (const auto device : devices)
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "capture device, name:" << device.name << ", id:" << device.id;
  if (devices.empty())
    throw std::runtime_error("no video capture devices available");
  rtc::google::capture::video::device_creator device_creator;
  std::shared_ptr<rtc::google::capture::video::device> capture_device =
      device_creator(devices.front().id);

  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication app(argc, argv);

  using frame_provider = client::ui::frame_provider_google_video_source;
  QQmlApplicationEngine engine;
  qmlRegisterType<frame_provider>("io.fubble.FrameProvider", 1, 0,
                                  "FrameProvider");
  const QUrl url(QStringLiteral("qrc:/main.qml"));
  engine.load(url);
  auto provider =
      engine.rootObjects().first()->findChild<frame_provider *>("provider");
  BOOST_ASSERT(provider);
  provider->set_source(capture_device.get());

  capture_device->start();

  // TODO deliver frames to qml
  // https://stackoverflow.com/questions/43854589/custom-source-property-for-videooutput-qml
  // https://doc.qt.io/qt-5/videooverview.html

  return app.exec();
}
