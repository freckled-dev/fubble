#include "audio_video_settings_model.hpp"
#include "chat_model.hpp"
#include "client/add_audio_to_connection.hpp"
#include "client/add_video_to_connection.hpp"
#include "client/audio_device_settings.hpp"
#include "client/audio_level_calculator.hpp"
#include "client/audio_tracks_volume.hpp"
#include "client/crash_catcher.hpp"
#include "client/desktop_sharing.hpp"
#include "client/factory.hpp"
#include "client/joiner.hpp"
#include "client/leaver.hpp"
#include "client/loopback_audio.hpp"
#include "client/own_audio_information.hpp"
#include "client/own_audio_track.hpp"
#include "client/own_media.hpp"
#include "client/own_microphone_tester.hpp"
#include "client/participant_creator_creator.hpp"
#include "client/peer_creator.hpp"
#include "client/peers.hpp"
#include "client/room_creator.hpp"
#include "client/rooms.hpp"
#include "client/tracks_adder.hpp"
#include "client/video_layout/video_layout.hpp"
#include "client/video_settings.hpp"
#include "error_model.hpp"
#include "executor_asio.hpp"
#include "gui_options.hpp"
#include "http/action_factory.hpp"
#include "http/connection_creator.hpp"
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
#include "own_media_model.hpp"
#include "participant_model.hpp"
#include "participants_model.hpp"
#include "participants_with_video_model.hpp"
#include "poll_asio_by_qt.hpp"
#include "room_model.hpp"
#include "rtc/google/asio_signalling_thread.hpp"
#include "rtc/google/audio_track.hpp"
#include "rtc/google/capture/audio/device.hpp"
#include "rtc/google/capture/audio/device_creator.hpp"
#include "rtc/google/capture/video/device.hpp"
#include "rtc/google/capture/video/enumerator.hpp"
#include "rtc/google/factory.hpp"
#include "rtc/google/log_webrtc_to_logging.hpp"
#include "share_desktop_model.hpp"
#include "signalling/client/client.hpp"
#include "signalling/client/connection_creator.hpp"
#include "signalling/json_message.hpp"
#include "temporary_room/net/client.hpp"
#include "ui/add_version_to_qml_context.hpp"
#include "ui/frame_provider_google_video_device.hpp"
#include "ui/frame_provider_google_video_frame.hpp"
#include "ui/log_qt_to_logging.hpp"
#include "utils/timer.hpp"
#include "utils/version.hpp"
#include "utils_model.hpp"
#include "websocket/connection_creator.hpp"
#include "websocket/connector.hpp"
#include <QApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QResource>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <fmt/format.h>
#include <fruit/fruit.h>
#include <thread>

fruit::Component<client::crash_catcher> create_crash_catcher_component() {
  return fruit::createComponent().install(client::crash_catcher::create);
}

int main(int argc, char *argv[]) {
  gui_options options_parser;
  auto config_check = options_parser.parse(argc, argv);
  if (!config_check)
    return 1;
  gui_config config = config_check.value();

  logging::add_console_log(config.general_.log_severity);
  logging::add_file_log(config.general_.log_severity);
  client::ui::log_qt_to_logging qt_logger;
  rtc::google::log_webrtc_to_logging webrtc_logger;

  logging::logger logger{"main"};

  BOOST_LOG_SEV(logger, logging::severity::info)
      << "starting up, version:" << utils::version();

  fruit::Injector<client::crash_catcher> injector{
      create_crash_catcher_component};
  if (config.general_.use_crash_catcher)
    injector.get<const client::crash_catcher &>();

  boost::asio::io_context context;
  boost::asio::executor executor{context.get_executor()};
  boost::executor_adaptor<executor_asio> boost_executor{context};
  rtc::google::asio_signalling_thread asio_signalling_thread{context};

  http::connection_creator connection_creator_{context};
  websocket::connection_creator websocket_connection_creator{context};
  websocket::connector_creator websocket_connector{
      context, websocket_connection_creator};

  // signalling
  BOOST_LOG_SEV(logger, logging::severity::trace) << "setting up signalling";
  signalling::json_message signalling_json;
  signalling::client::connection_creator signalling_connection_creator{
      context, boost_executor, signalling_json};
  signalling::client::client::connect_information connect_information{
      config.general_.use_ssl, config.general_.host, config.general_.service,
      "/api/signalling/v0/"};
  signalling::client::factory_impl signalling_client_creator{
      websocket_connector, signalling_connection_creator, connect_information};
  utils::one_shot_timer signalling_client_creator_timer{
      context, std::chrono::seconds(1)};
  signalling::client::factory_reconnecting
      signalling_client_creator_reconnecting{signalling_client_creator,
                                             signalling_client_creator_timer};

  // session, matrix and temporary_room
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "setting up matrix and temporary_room";
  http::server http_matrix_client_server{config.general_.host,
                                         config.general_.service};
  http_matrix_client_server.secure = config.general_.use_ssl;
  http::fields http_matrix_client_fields{http_matrix_client_server};
  http_matrix_client_fields.target_prefix = "/api/matrix/v0/_matrix/client/r0/";
  http::action_factory action_factory_{connection_creator_};
  http::client_factory http_matrix_client_factory{
      action_factory_, http_matrix_client_server, http_matrix_client_fields};

  http::server http_temporary_room_client_server{config.general_.host,
                                                 config.general_.service};
  http_temporary_room_client_server.secure = config.general_.use_ssl;
  http::fields http_temporary_room_client_fields{
      http_temporary_room_client_server};
  http_temporary_room_client_fields.target_prefix = "/api/temporary_room/v0/";
  http::client http_client_temporary_room{action_factory_,
                                          http_temporary_room_client_server,
                                          http_temporary_room_client_fields};
  temporary_room::net::client temporary_room_client{http_client_temporary_room};
  matrix::factory matrix_factory;
  matrix::client_factory matrix_client_factory{matrix_factory,
                                               http_matrix_client_factory};
  matrix::authentification matrix_authentification{http_matrix_client_factory,
                                                   matrix_client_factory};

  BOOST_LOG_SEV(logger, logging::severity::trace) << "setting up webrtc";
  rtc::google::settings rtc_settings;
  rtc_settings.use_ip_v6 = config.general_.use_ipv6;
  rtc::google::factory rtc_factory{rtc_settings,
                                   asio_signalling_thread.get_native()};
  client::peer_creator peer_creator{
      boost_executor, signalling_client_creator_reconnecting, rtc_factory};
  auto tracks_adder = std::make_shared<client::tracks_adder>();

  // audio
  BOOST_LOG_SEV(logger, logging::severity::trace) << "setting up audio device";
  rtc::google::capture::audio::device_creator audio_device_creator{rtc_factory};
  std::unique_ptr<rtc::google::capture::audio::device> audio_device =
      audio_device_creator.create();
  auto &rtc_audio_devices = rtc_factory.get_audio_devices();
  client::rooms rooms;
  auto own_audio_track =
      client::own_audio_track::create(rtc_factory, audio_device->get_source());
  auto audio_track_adder = std::make_shared<client::add_audio_to_connection>(
      own_audio_track->get_track());
  auto audio_tracks_volume = client::audio_tracks_volume::create(
      rooms, *tracks_adder, audio_track_adder, *own_audio_track);
  std::shared_ptr<rtc::google::audio_track> settings_audio_track =
      rtc_factory.create_audio_track(audio_device->get_source());
  client::loopback_audio_impl_factory loopback_audio_test_factory{
      rtc_factory, settings_audio_track};
  client::loopback_audio_impl loopback_audio{rtc_factory,
                                             own_audio_track->get_track()};
  client::audio_level_calculator_factory audio_level_calculator_factory_{
      boost_executor};
  client::own_audio_information own_audio_information_{
      audio_level_calculator_factory_, loopback_audio};
  client::loopback_audio_noop_if_disabled loopback_audio_test{
      loopback_audio_test_factory};
  auto own_microphone_tester = client::own_microphone_tester::create(
      loopback_audio_test, *audio_tracks_volume);
  client::own_audio_information own_audio_test_information_{
      audio_level_calculator_factory_, loopback_audio_test};

  // video
  BOOST_LOG_SEV(logger, logging::severity::trace) << "setting up video device";
  std::unique_ptr<utils::interval_timer> video_enumerator_timer;
  std::unique_ptr<rtc::video_devices> video_enumerator_google;
  std::unique_ptr<rtc::video_devices> video_enumerator;
  if (config.general_.video_support) {
    video_enumerator_google =
        std::make_unique<rtc::google::capture::video::enumerator>();
    video_enumerator_timer = std::make_unique<utils::interval_timer>(
        context, std::chrono::seconds(1));
    video_enumerator = rtc::video_devices::create_interval_enumerating(
        *video_enumerator_google, *video_enumerator_timer);
  } else {
    video_enumerator = std::make_unique<rtc::video_devices_noop>();
  }
  rtc::google::capture::video::device_factory video_device_creator;
  auto add_video_to_connection_factory_ =
      std::make_shared<client::add_video_to_connection_factory_impl>(
          rtc_factory);
  auto own_videos_ = client::own_video::create();
  auto video_settings = std::make_shared<client::video_settings>(
      *video_enumerator, video_device_creator, *own_videos_, *tracks_adder,
      *add_video_to_connection_factory_);

  // desktop
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "setting up desktop sharing";
  auto timer_factory = std::make_shared<utils::timer_factory>(context);
  std::shared_ptr<client::desktop_sharing> desktop_sharing =
      client::desktop_sharing::create(timer_factory, tracks_adder,
                                      add_video_to_connection_factory_,
                                      video_settings);

  // client
  BOOST_LOG_SEV(logger, logging::severity::trace) << "setting up client";
  client::factory client_factory{context};
  client::own_media own_media{*own_audio_track, *own_videos_};
  client::participant_creator_creator participant_creator_creator{
      client_factory, peer_creator, *tracks_adder, own_media, desktop_sharing};
  client::room_creator client_room_creator{participant_creator_creator};
  client::joiner joiner{client_room_creator, rooms, matrix_authentification,
                        temporary_room_client};
  client::leaver leaver{rooms};

  BOOST_LOG_SEV(logger, logging::severity::debug) << "starting qt";

  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  std::vector<char *> argv_adopted{argv, argv + argc};
#if BOOST_OS_WINDOWS
  std::string arg_plaform = "-platform";
  std::string arg_fontengine_freetype = "windows:fontengine=freetype";
  argv_adopted.push_back(arg_plaform.data());
  argv_adopted.push_back(arg_fontengine_freetype.data());
#endif
  int argc_adopted = argv_adopted.size();
  // do not use QGuiApplication. Charts needs the widgets QApplication
  // https://doc.qt.io/qt-5/qtcharts-qmlmodule.html
  QApplication app(argc_adopted, argv_adopted.data());
  app.setOrganizationName("Freckled OG");
  app.setOrganizationDomain("freckled.dev");
  app.setApplicationName("Fubble");

  // load font
  QString font_path_share =
      QCoreApplication::applicationDirPath() + "/../share/fubble/resources.rcc";
  QString font_path_executable =
      QCoreApplication::applicationDirPath() + "/resources.rcc";
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "font rcc path_share:" << font_path_share.toStdString()
      << ", path_executable:" << font_path_executable.toStdString();
  bool loaded = QResource::registerResource(font_path_share);
  if (!loaded)
    loaded |= QResource::registerResource(font_path_executable);
  BOOST_ASSERT(loaded);
  if (!loaded)
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "could not load external resources. This might lead to fatal "
           "errors!";

  // applying material style
  QQuickStyle::setStyle("Material");

  app.setWindowIcon(QIcon(":/images/fubble.svg"));
  // we are regestering with full namespace. so use full namespace in signals
  // and properties
  qRegisterMetaType<client::ui::frame_provider_google_video_source *>();
  qRegisterMetaType<client::ui::frame_provider_google_video_device *>();
  qRegisterMetaType<client::room_model *>();
  qRegisterMetaType<client::participant_model *>();
  qRegisterMetaType<client::participants_model *>();
  qRegisterMetaType<client::participants_with_video_model *>();
  qRegisterMetaType<client::join_model *>();
  qRegisterMetaType<client::own_media_model *>();
  qRegisterMetaType<client::share_desktop_model *>();
  qRegisterMetaType<client::error_model *>();
  qRegisterMetaType<client::utils_model *>();
  qRegisterMetaType<client::leave_model *>();
  qRegisterMetaType<client::audio_video_settings_model *>();
  qRegisterMetaType<client::devices_model *>();

  // https://doc.qt.io/qt-5/qtqml-cppintegration-overview.html#choosing-the-correct-integration-method-between-c-and-qml
  qmlRegisterUncreatableType<client::room_model>(
      "io.fubble", 1, 0, "RoomModel", "can't instance client::room_model");
  qmlRegisterUncreatableType<client::participant_model>(
      "io.fubble", 1, 0, "ParticipantModel",
      "can't instance client::participant_model");
  qmlRegisterUncreatableType<client::join_model>(
      "io.fubble", 1, 0, "JoinModel", "can't instance client::join_model");
  qmlRegisterUncreatableType<client::share_desktop_model>(
      "io.fubble", 1, 0, "ShareDesktopModel",
      "can't instance client::share_desktop_model");
  qmlRegisterUncreatableType<client::utils_model>(
      "io.fubble", 1, 0, "UtilsModel", "can't instance client::utils_model");
  qmlRegisterUncreatableType<client::error_model>(
      "io.fubble", 1, 0, "ErrorModel", "can't instance client::error_model");
  qmlRegisterUncreatableType<client::leave_model>(
      "io.fubble", 1, 0, "LeaveModel", "can't instance client::leave_model");
  qmlRegisterUncreatableType<client::chat_model>(
      "io.fubble", 1, 0, "ChatModel", "can't instance client::chat_model");
  qmlRegisterUncreatableType<client::chat_messages_model>(
      "io.fubble", 1, 0, "ChatMessagesModel",
      "can't instance client::chat_messages_model");
  qmlRegisterUncreatableType<client::audio_video_settings_model>(
      "io.fubble", 1, 0, "AudioVideoSettingsModel",
      "can't instance client::audio_video_settings_model");
  qmlRegisterUncreatableType<client::own_media_model>(
      "io.fubble", 1, 0, "OwnMediaModel",
      "can't instance client::own_media_model");
  qmlRegisterUncreatableType<client::devices_model>(
      "io.fubble", 1, 0, "DevicesModel",
      "can't instance client::devices_model");
  qmlRegisterType<video_layout>("io.fubble", 1, 0, "VideoLayout");

  QQmlApplicationEngine engine;
  client::audio_device_settings audio_settings{rtc_audio_devices};
  client::model_creator model_creator{
      audio_level_calculator_factory_, audio_settings, *video_settings,
      own_audio_information_, *audio_tracks_volume};
  client::error_model error_model;
  client::utils_model utils_model;
  client::join_model join_model{model_creator, error_model, joiner};
  client::share_desktop_model share_desktop_model{desktop_sharing};
  client::leave_model leave_model{leaver};
  client::own_media_model own_media_model{audio_settings,
                                          *video_settings,
                                          *own_microphone_tester,
                                          *audio_tracks_volume,
                                          own_audio_information_,
                                          own_audio_test_information_,
                                          own_media};
  client::audio_video_settings_model audio_video_settings_model{
      rtc_audio_devices, *video_enumerator, video_device_creator,
      audio_settings,    *video_settings,   error_model};
  //  works from 5.14 onwards
  // engine.setInitialProperties(...)
  //  setContextProperty sets it globaly not as property of the window
  QQmlContext *qml_context = engine.rootContext();
  qml_context->setContextProperty("joinModelFromCpp", &join_model);
  qml_context->setContextProperty("errorModelFromCpp", &error_model);
  qml_context->setContextProperty("ownMediaModelFromCpp", &own_media_model);
  qml_context->setContextProperty("utilsModelFromCpp", &utils_model);
  qml_context->setContextProperty("leaveModelFromCpp", &leave_model);
  qml_context->setContextProperty("audioVideoModelFromCpp",
                                  &audio_video_settings_model);
  qml_context->setContextProperty("shareDesktopModelFromCpp",
                                  &share_desktop_model);
  client::ui::add_version_to_qml_context version_adder{*qml_context};
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
}
