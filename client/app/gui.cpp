#include "chat_model.hpp"
#include "client/add_audio_to_connection.hpp"
#include "client/add_video_to_connection.hpp"
#include "client/joiner.hpp"
#include "client/leaver.hpp"
#include "client/own_media.hpp"
#include "client/participant_creator_creator.hpp"
#include "client/peer_creator.hpp"
#include "client/peers.hpp"
#include "client/room_creator.hpp"
#include "client/rooms.hpp"
#include "client/tracks_adder.hpp"
#include "error_model.hpp"
#include "executor_asio.hpp"
#include "gui_options.hpp"
#include "join_model.hpp"
#include "leave_model.hpp"
#include "logging/initialser.hpp"
#include "logging/logger.hpp"
#include "matrix/authentification.hpp"
#include "matrix/client.hpp"
#include "matrix/client_factory.hpp"
#include "matrix/factory.hpp"
#include "matrix/rooms.hpp"
#include "model_creator.hpp"
#include "poll_asio_by_qt.hpp"
#include "room_model.hpp"
#include "rtc/google/asio_signalling_thread.hpp"
#include "rtc/google/capture/audio/device.hpp"
#include "rtc/google/capture/audio/device_creator.hpp"
#include "rtc/google/capture/video/device.hpp"
#include "rtc/google/capture/video/device_creator.hpp"
#include "rtc/google/capture/video/enumerator.hpp"
#include "rtc/google/factory.hpp"
#include "signalling/client/client_creator.hpp"
#include "signalling/client/connection_creator.hpp"
#include "signalling/json_message.hpp"
#include "temporary_room/net/client.hpp"
#include "ui/frame_provider_google_video_frame.hpp"
#include "websocket/connection_creator.hpp"
#include "websocket/connector.hpp"
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <fmt/format.h>
#include <thread>

int main(int argc, char *argv[]) {
  gui_options options_parser;
  auto config_check = options_parser.parse(argc, argv);
  if (!config_check)
    return 1;
  gui_config config = config_check.value();

  logging::add_console_log();
  logging::logger logger{"main"};
  BOOST_LOG_SEV(logger, logging::severity::debug) << "starting up";

  boost::asio::io_context context;
  boost::asio::executor executor{context.get_executor()};
  boost::executor_adaptor<executor_asio> boost_executor{context};
  rtc::google::asio_signalling_thread asio_signalling_thread{context};

  websocket::connection_creator websocket_connection_creator{context};
  websocket::connector_creator websocket_connector{
      context, websocket_connection_creator};

  // signalling
  signalling::json_message signalling_json;
  signalling::client::connection_creator signalling_connection_creator{
      context, boost_executor, signalling_json};
  signalling::client::client::connect_information connect_information{
      config.general_.host, config.general_.service, "/api/signalling/v0/"};
  signalling::client::client_creator client_creator{
      websocket_connector, signalling_connection_creator, connect_information};

  // session, matrix and temporary_room
  http::server http_matrix_client_server{config.general_.host,
                                         config.general_.service};
  http::fields http_matrix_client_fields{http_matrix_client_server};
  http_matrix_client_fields.target_prefix = "/api/matrix/v0/_matrix/client/r0/";
  http::client_factory http_matrix_client_factory{
      context, http_matrix_client_server, http_matrix_client_fields};

  http::server http_temporary_room_client_server{config.general_.host,
                                                 config.general_.service};
  http::fields http_temporary_room_client_fields{
      http_temporary_room_client_server};
  http_temporary_room_client_fields.target_prefix = "/api/temporary_room/v0/";
  http::client http_client_temporary_room{context,
                                          http_temporary_room_client_server,
                                          http_temporary_room_client_fields};
  temporary_room::net::client temporary_room_client{http_client_temporary_room};
  matrix::factory matrix_factory;
  matrix::client_factory matrix_client_factory{matrix_factory,
                                               http_matrix_client_factory};
  matrix::authentification matrix_authentification{http_matrix_client_factory,
                                                   matrix_client_factory};
  rtc::google::settings rtc_settings;
  rtc_settings.use_ip_v6 = config.general_.use_ipv6;
  rtc::google::factory rtc_connection_creator{
      rtc_settings, asio_signalling_thread.get_native()};
  client::peer_creator peer_creator{boost_executor, client_creator,
                                    rtc_connection_creator};
  client::tracks_adder tracks_adder;

#if 1
  // audio
  rtc::google::capture::audio::device_creator audio_device_creator{
      rtc_connection_creator};
  auto audio_device = audio_device_creator.create();
  client::add_audio_to_connection audio_track_adder(rtc_connection_creator,
                                                    *audio_device);
  tracks_adder.add(audio_track_adder);
#endif
  client::rooms rooms;
  client::own_media own_media;

  // video
  rtc::google::capture::video::enumerator enumerator;
  auto devices = enumerator();
  for (const auto device : devices)
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "capture device, name:" << device.name << ", id:" << device.id;
  if (devices.empty())
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << "there are no capture devices";
  rtc::google::capture::video::device_creator device_creator;
  std::shared_ptr<rtc::google::capture::video::device> capture_device;
  for (const auto &current_device : devices) {
    try {
      capture_device = device_creator(current_device.id);
      capture_device->start();
      break;
    } catch (const std::exception &error) {
      BOOST_LOG_SEV(logger, logging::severity::warning) << fmt::format(
          "could not start capturing from device, id:'{}' error:{}",
          current_device.id, error.what());
    }
  }
  std::unique_ptr<client::add_video_to_connection> video_track_adder;
  if (!capture_device) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << "no capture device could be initialsed";
  } else {
    video_track_adder = std::make_unique<client::add_video_to_connection>(
        rtc_connection_creator, capture_device);
    tracks_adder.add(*video_track_adder);
    own_media.add_video(*capture_device);
  }
  client::participant_creator_creator participant_creator_creator{
      peer_creator, tracks_adder, own_media};
  client::room_creator client_room_creator{participant_creator_creator};
  client::joiner joiner{client_room_creator, rooms, matrix_authentification,
                        temporary_room_client};
  client::leaver leaver{rooms};

  BOOST_LOG_SEV(logger, logging::severity::debug) << "starting qt";

  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication app(argc, argv);
  app.setOrganizationName("Freckled OG");
  app.setOrganizationDomain("freckled.dev");
  app.setApplicationName("Fubble");

  // applying material style
  QQuickStyle::setStyle("Material");

  app.setWindowIcon(QIcon(":/pics/fubble.svg"));
  // we are regestering with full namespace. so use full namespace in signals
  // and properties
  qRegisterMetaType<client::ui::frame_provider_google_video_source *>();
  qRegisterMetaType<client::room_model *>();
  qRegisterMetaType<client::participant_model *>();
  qRegisterMetaType<client::participants_model *>();
  qRegisterMetaType<client::participants_with_video_model *>();
  qRegisterMetaType<client::join_model *>();
  qRegisterMetaType<client::error_model *>();
  qRegisterMetaType<client::leave_model *>();

  // https://doc.qt.io/qt-5/qtqml-cppintegration-overview.html#choosing-the-correct-integration-method-between-c-and-qml
  qmlRegisterUncreatableType<client::room_model>(
      "io.fubble", 1, 0, "RoomModel", "can't instance client::room_model");
  qmlRegisterUncreatableType<client::participant_model>(
      "io.fubble", 1, 0, "ParticipantModel",
      "can't instance client::participant_model");
  qmlRegisterUncreatableType<client::join_model>(
      "io.fubble", 1, 0, "JoinModel", "can't instance client::join_model");
  qmlRegisterUncreatableType<client::error_model>(
      "io.fubble", 1, 0, "ErrorModel", "can't instance client::error_model");
  qmlRegisterUncreatableType<client::leave_model>(
      "io.fubble", 1, 0, "LeaveModel", "can't instance client::leave_model");
  qmlRegisterUncreatableType<client::chat_model>(
      "io.fubble", 1, 0, "ChatModel", "can't instance client::chat_model");
  qmlRegisterUncreatableType<client::chat_messages_model>(
      "io.fubble", 1, 0, "ChatMessagesModel",
      "can't instance client::chat_messages_model");

  QQmlApplicationEngine engine;
  client::model_creator model_creator;
  client::error_model error_model;
  client::join_model join_model{model_creator, error_model, joiner, own_media};
  client::leave_model leave_model{leaver};
  //  works from 5.14 onwards
  // engine.setInitialProperties(...)
  //  setContextProperty sets it globaly not as property of the window
  engine.rootContext()->setContextProperty("joinModelFromCpp", &join_model);
  engine.rootContext()->setContextProperty("errorModelFromCpp", &error_model);
  engine.rootContext()->setContextProperty("leaveModelFromCpp", &leave_model);
  //  seems not to do it either
  // QVariant property{qMetaTypeId<client::join_model *>(), &join_model};
  // engine.setProperty("joinModel", property);

  const QUrl url(QStringLiteral("qrc:/FubbleApplication.qml"));
  BOOST_LOG_SEV(logger, logging::severity::debug) << "loading qml";
  engine.load(url);
  BOOST_LOG_SEV(logger, logging::severity::debug) << "loaded qml";

  client::poll_asio_by_qt asio_poller{context};
  asio_poller.run();

  auto result = app.exec();
  BOOST_LOG_SEV(logger, logging::severity::debug) << "gui stopped";
  context.stop();
  return result;
  return 0;
}
