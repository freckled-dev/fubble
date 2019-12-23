#include "executor_asio.hpp"
#include "join_model.hpp"
#include "joiner.hpp"
#include "logging/initialser.hpp"
#include "logging/logger.hpp"
#include "rtc/google/capture/video/device.hpp"
#include "rtc/google/capture/video/device_creator.hpp"
#include "rtc/google/capture/video/enumerator.hpp"
#include "rtc/google/factory.hpp"
#include "signalling/client/client_creator.hpp"
#include "signalling/client/connection_creator.hpp"
#include "signalling/json_message.hpp"
#include "ui/frame_provider_google_video_frame.hpp"
#include "websocket/connection_creator.hpp"
#include "websocket/connector.hpp"
#include <boost/asio/io_context.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#pragma GCC diagnostic pop

int main(int argc, char *argv[]) {
  logging::add_console_log();
  logging::logger logger;
  BOOST_LOG_SEV(logger, logging::severity::debug) << "starting up";

  boost::asio::io_context context;
  boost::asio::executor executor{context.get_executor()};
  boost::executor_adaptor<executor_asio> boost_executor{context};

  websocket::connection_creator websocket_connection_creator{context};
  websocket::connector websocket_connector{context, boost_executor,
                                           websocket_connection_creator};

  signalling::json_message signalling_json;
  signalling::client::connection_creator signalling_connection_creator{
      context, boost_executor, signalling_json};
  signalling::client::client_creator client_creator{
      boost_executor, websocket_connector, signalling_connection_creator};

  rtc::google::factory rtc_connection_creator;
  client::joiner joiner{client_creator, rtc_connection_creator};

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

  BOOST_LOG_SEV(logger, logging::severity::debug) << "starting qt";

  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication app(argc, argv);
  using frame_provider = client::ui::frame_provider_google_video_source;
  QQmlApplicationEngine engine;
  client::join_model backend(joiner);
  engine.rootContext()->setContextProperty("join", &backend);
  qmlRegisterType<frame_provider>("io.fubble.FrameProvider", 1, 0,
                                  "FrameProvider");
  const QUrl url(QStringLiteral("qrc:/main.qml"));
  BOOST_LOG_SEV(logger, logging::severity::debug) << "loading qml";
  engine.load(url);
  BOOST_LOG_SEV(logger, logging::severity::debug) << "loaded qml";
  auto provider =
      engine.rootObjects().first()->findChild<frame_provider *>("provider");
  BOOST_ASSERT(provider);
  provider->set_source(capture_device.get());

  capture_device->start();

  return app.exec();
}
