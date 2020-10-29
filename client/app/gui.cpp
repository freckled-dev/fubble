#include "audio_video_settings_model.hpp"
#include "chat_model.hpp"
#include "client/add_audio_to_connection.hpp"
#include "client/add_video_to_connection.hpp"
#include "client/audio_device_settings.hpp"
#include "client/audio_level_calculator.hpp"
#include "client/audio_module.hpp"
#include "client/audio_tracks_volume.hpp"
#include "client/crash_catcher.hpp"
#include "client/desktop_sharing.hpp"
#include "client/factory.hpp"
#include "client/joiner.hpp"
#include "client/leaver.hpp"
#include "client/loopback_audio.hpp"
#include "client/mute_deaf_communicator.hpp"
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
#include "gui_options.hpp"
#include "http/action_factory.hpp"
#include "http/client_module.hpp"
#include "http/connection_creator.hpp"
#include "join_model.hpp"
#include "leave_model.hpp"
#include "log_module.hpp"
#include "logging/logger.hpp"
#include "matrix/module.hpp"
#include "model_creator.hpp"
#include "own_media_model.hpp"
#include "participant_model.hpp"
#include "participants_model.hpp"
#include "participants_with_video_model.hpp"
#include "poll_asio_by_qt.hpp"
#include "room_model.hpp"
#include "rtc/google/audio_track.hpp"
#include "rtc/google/capture/audio/device.hpp"
#include "rtc/google/capture/video/device.hpp"
#include "rtc/google/capture/video/enumerator.hpp"
#include "rtc/google/factory.hpp"
#include "rtc/google/module.hpp"
#include "share_desktop_model.hpp"
#include "signalling/client_module.hpp"
#include "temporary_room/client_module.hpp"
#include "ui/add_version_to_qml_context.hpp"
#include "ui/frame_provider_google_video_device.hpp"
#include "ui/frame_provider_google_video_frame.hpp"
#include "utils/executor_module.hpp"
#include "utils/timer.hpp"
#include "utils/version.hpp"
#include "utils_model.hpp"
#include <QApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QResource>

int main(int argc, char *argv[]) {
  gui_options options_parser;
  auto config_check = options_parser.parse(argc, argv);
  if (!config_check)
    return 1;
  gui_config config = config_check.value();

  client::log_module log_module_{config};
  logging::logger logger{"main"};

  BOOST_LOG_SEV(logger, logging::severity::info)
      << "starting up, fubble_version:" << utils::version()
      << ", qt_version:" << qVersion();

  std::unique_ptr<client::crash_catcher> crash_catcher;
  if (config.general_.use_crash_catcher)
    crash_catcher = client::crash_catcher::create();

  auto executor_module_ = std::make_shared<utils::executor_module>();
  boost::asio::io_context &context(*executor_module_->get_io_context());
  std::shared_ptr<boost::executor> boost_executor =
      executor_module_->get_boost_executor();

  auto http_client_module =
      std::make_shared<http::client_module>(executor_module_);

  // signalling
  BOOST_LOG_SEV(logger, logging::severity::debug) << "setting up signalling";
  signalling::client_module::config signalling_config;
  signalling_config.host = config.general_.host;
  signalling_config.secure = config.general_.use_ssl;
  signalling_config.service = config.general_.service;
  auto signalling_module = std::make_shared<signalling::client_module>(
      executor_module_, signalling_config);

  // matrix
  BOOST_LOG_SEV(logger, logging::severity::debug) << "setting up matrix";
  matrix::module::config matrix_config;
  matrix_config.host = config.general_.host;
  matrix_config.service = config.general_.service;
  matrix_config.use_ssl = config.general_.use_ssl;
  matrix::module matrix_module{executor_module_, http_client_module,
                               matrix_config};

  // temporary_room
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "setting up temporary_room";
  temporary_room::client_module::config temporary_room_config;
  temporary_room_config.host = config.general_.host;
  temporary_room_config.service = config.general_.service;
  temporary_room_config.use_ssl = config.general_.use_ssl;
  auto temporary_room_module = std::make_shared<temporary_room::client_module>(
      http_client_module, temporary_room_config);

  BOOST_LOG_SEV(logger, logging::severity::trace) << "setting up webrtc";
  rtc::google::settings rtc_settings;
  rtc_settings.use_ip_v6 = config.general_.use_ipv6;
  auto rtc_module =
      std::make_shared<rtc::google::module>(executor_module_, rtc_settings);

  client::peer_creator peer_creator{*boost_executor,
                                    *signalling_module->get_client_creator(),
                                    *rtc_module->get_factory()};
  auto tracks_adder = std::make_shared<client::tracks_adder>();

  // TODO
  std::shared_ptr<client::rooms> rooms = std::make_shared<client::rooms>();

  // audio
  BOOST_LOG_SEV(logger, logging::severity::debug) << "setting up client_audio";
  client::audio_module::config client_audio_config;
  std::shared_ptr<client::audio_module> client_audio_module =
      std::make_shared<client::audio_module>(executor_module_, rtc_module,
                                             client_audio_config);
  auto &rtc_audio_devices = rtc_module->get_factory()->get_audio_devices();
  // TODO audio_settings_module
  // TODO adds audio_track_adder to tracks_adder - refactor!
  std::shared_ptr<client::audio_tracks_volume> audio_tracks_volume =
      client::audio_tracks_volume::create(
          *rooms, *tracks_adder,
          client_audio_module->get_own_audio_track_adder(),
          *client_audio_module->get_own_audio_track());
  std::shared_ptr<rtc::google::audio_track> settings_audio_track =
      rtc_module->get_factory()->create_audio_track(
          rtc_module->get_audio_device()->get_source());
  client::loopback_audio_impl_factory loopback_audio_test_factory{
      *rtc_module->get_factory(), settings_audio_track, boost_executor};
  client::loopback_audio_noop_if_disabled loopback_audio_test{
      loopback_audio_test_factory};
  auto own_microphone_tester = client::own_microphone_tester::create(
      loopback_audio_test, *audio_tracks_volume);
  client::own_audio_information own_audio_test_information_{
      *client_audio_module->get_audio_level_calculator_factory(),
      loopback_audio_test};

  std::shared_ptr<client::mute_deaf_communicator> mute_deaf_communicator_ =
      client::mute_deaf_communicator::create(rooms, audio_tracks_volume);
  client::audio_device_settings audio_settings{rtc_audio_devices};

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
  auto add_video_to_connection_factory_ =
      std::make_shared<client::add_video_to_connection_factory_impl>(
          *rtc_module->get_factory());
  auto own_videos_ = client::own_video::create();
  auto video_settings = std::make_shared<client::video_settings>(
      *video_enumerator, *rtc_module->get_video_device_creator(), *own_videos_,
      *tracks_adder, *add_video_to_connection_factory_);

  // leaver
  auto leaver = std::make_shared<client::leaver>(*rooms);

  // desktop
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "setting up desktop sharing";
  auto timer_factory = std::make_shared<utils::timer_factory>(context);
  std::shared_ptr<client::desktop_sharing> desktop_sharing =
      client::desktop_sharing::create(timer_factory, tracks_adder,
                                      add_video_to_connection_factory_,
                                      video_settings, leaver);
  std::shared_ptr<client::desktop_sharing_previews> desktop_sharing_previews =
      client::desktop_sharing_previews::create(timer_factory);

  // version
  http::server http_version_client_server{config.general_.host,
                                          config.general_.service};
  http_version_client_server.secure = config.general_.use_ssl;
  http::fields http_version_client_fields{http_version_client_server};
  http_version_client_fields.target_prefix = "/api/version/v0/";
  std::shared_ptr<http::client> version_http_client =
      std::make_shared<http::client>(
          http_client_module->get_action_factory(),
          std::make_pair(http_version_client_server,
                         http_version_client_fields));
  std::shared_ptr<version::getter> version_getter =
      version::getter::create(version_http_client);

  // client
  BOOST_LOG_SEV(logger, logging::severity::trace) << "setting up client";
  client::factory client_factory{context};
  client::own_media own_media{*client_audio_module->get_own_audio_track(),
                              *own_videos_};
  client::participant_creator_creator participant_creator_creator{
      client_factory, peer_creator, *tracks_adder, own_media, desktop_sharing};
  client::room_creator client_room_creator{participant_creator_creator};
  client::joiner joiner{client_room_creator, *rooms,
                        *matrix_module.get_authentification(),
                        *temporary_room_module->get_client(), version_getter};

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
  client::model_creator model_creator{
      *client_audio_module->get_audio_level_calculator_factory(),
      audio_settings,
      *video_settings,
      *client_audio_module->get_own_audio_information(),
      *audio_tracks_volume,
      mute_deaf_communicator_};
  client::error_model error_model;
  client::utils_model utils_model;
  client::join_model join_model{model_creator, error_model, joiner};
  client::share_desktop_model share_desktop_model{desktop_sharing,
                                                  desktop_sharing_previews};
  client::leave_model leave_model{*leaver};
  client::own_media_model own_media_model{
      audio_settings,
      *video_settings,
      *own_microphone_tester,
      *audio_tracks_volume,
      *client_audio_module->get_own_audio_information(),
      own_audio_test_information_,
      own_media};
  client::audio_video_settings_model audio_video_settings_model{
      rtc_audio_devices,
      *video_enumerator,
      *rtc_module->get_video_device_creator(),
      audio_settings,
      *video_settings,
      error_model,
      timer_factory};
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
