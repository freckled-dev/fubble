#include "add_video_to_connection.hpp"
#include "executor_asio.hpp"
#include "join_model.hpp"
#include "joiner.hpp"
#include "logging/initialser.hpp"
#include "logging/logger.hpp"
#include "participant_creator_creator.hpp"
#include "peer_creator.hpp"
#include "peers.hpp"
#include "room_creator.hpp"
#include "room_model.hpp"
#include "rooms.hpp"
#include "rtc/google/capture/video/device.hpp"
#include "rtc/google/capture/video/device_creator.hpp"
#include "rtc/google/capture/video/enumerator.hpp"
#include "rtc/google/factory.hpp"
#include "signalling/client/client_creator.hpp"
#include "signalling/client/connection_creator.hpp"
#include "signalling/json_message.hpp"
#include "ui/executor_qt.hpp"
#include "ui/frame_provider_google_video_frame.hpp"
#include "videos_model.hpp"
#include "websocket/connection_creator.hpp"
#include "websocket/connector.hpp"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <fmt/format.h>
#include <thread>

int main(int argc, char *argv[]) {
  logging::add_console_log();
  logging::logger logger;
  BOOST_LOG_SEV(logger, logging::severity::debug) << "starting up";

  boost::asio::io_context context;
  boost::asio::executor executor{context.get_executor()};
  boost::executor_adaptor<executor_asio> boost_executor{context};
  boost::executor_adaptor<client::ui::executor_qt> qt_executor;

  websocket::connection_creator websocket_connection_creator{context};
  websocket::connector_creator websocket_connector{
      context, websocket_connection_creator};

  signalling::json_message signalling_json;
  signalling::client::connection_creator signalling_connection_creator{
      context, boost_executor, signalling_json};
  signalling::client::client::connect_information connect_information{
      "localhost", "8000"};
  signalling::client::client_creator client_creator{
      websocket_connector, signalling_connection_creator, connect_information};

  rtc::google::factory rtc_connection_creator;
  client::peer_creator peer_creator{boost_executor, client_creator,
                                    rtc_connection_creator};

  rtc::google::capture::video::enumerator enumerator;
  auto devices = enumerator();
  for (const auto device : devices)
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "capture device, name:" << device.name << ", id:" << device.id;
  if (devices.empty())
    throw std::runtime_error("no video capture devices available");
  rtc::google::capture::video::device_creator device_creator;
  std::shared_ptr<rtc::google::capture::video::device> capture_device;
  for (const auto &current_device : devices) {
    capture_device = device_creator(current_device.id);
    try {
      capture_device->start();
      break;
    } catch (const std::runtime_error &error) {
      BOOST_LOG_SEV(logger, logging::severity::warning) << fmt::format(
          "could not start capturing from device, id:'{}' error:{}",
          current_device.id, error.what());
    }
  }
  if (!capture_device)
    throw std::runtime_error("no camera device available");

  client::peers peers;
  client::add_video_to_connection track_adder(rtc_connection_creator,
                                              capture_device);
  client::rooms rooms;
  client::participant_creator_creator participant_creator_creator{peer_creator};
  client::room_creator client_room_creator{participant_creator_creator};
  client::joiner joiner{executor, client_room_creator, rooms};

  BOOST_LOG_SEV(logger, logging::severity::debug) << "starting qt";

  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication app(argc, argv);
  // we are regestering with full namespace. so use full namespace in signals
  // and properties
  qRegisterMetaType<client::ui::frame_provider_google_video_source *>();
  qRegisterMetaType<client::room_model *>();
  qRegisterMetaType<client::participant_model *>();
  qRegisterMetaType<client::participants_model *>();

  qmlRegisterUncreatableType<client::room_model>("io.fubble", 1, 0, "RoomModel",
                                                 "some message");
  qmlRegisterUncreatableType<client::participant_model>(
      "io.fubble", 1, 0, "ParticipantModel", "some message");
  QQmlApplicationEngine engine;
  client::join_model join_model{joiner};
  engine.rootContext()->setContextProperty("joinModel", &join_model);
  client::videos_model videos_model{peers};
  engine.rootContext()->setContextProperty("videosModel", &videos_model);

  const QUrl url(QStringLiteral("qrc:/main.qml"));
  BOOST_LOG_SEV(logger, logging::severity::debug) << "loading qml";
  engine.load(url);
  BOOST_LOG_SEV(logger, logging::severity::debug) << "loaded qml";
  videos_model.get_own_video()->set_source(capture_device.get());

  auto work_guard = boost::asio::make_work_guard(context);
  std::thread asio_thread{[&context, &logger] {
    BOOST_LOG_SEV(logger, logging::severity::debug) << "context.run()";
    context.run();
    BOOST_LOG_SEV(logger, logging::severity::debug) << "context.run() is over";
  }};
  auto result = app.exec();
  BOOST_LOG_SEV(logger, logging::severity::debug) << "gui stopped";
  work_guard.reset();
  peers.close();
  context.stop();
  if (asio_thread.joinable())
    asio_thread.join();
  return result;
}
