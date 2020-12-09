#include "ui_module.hpp"
#include "audio_video_settings_model.hpp"
#include "chat_model.hpp"
#include "error_model.hpp"
#include "fubble/client/audio_tracks_volume.hpp"
#include "join_model.hpp"
#include "language_settings_model.hpp"
#include "leave_model.hpp"
#include "model_creator.hpp"
#include "own_media_model.hpp"
#include "participant_model.hpp"
#include "participants_model.hpp"
#include "participants_with_video_model.hpp"
#include "room_model.hpp"
#include "rooms_model.hpp"
#include "share_desktop_model.hpp"
#include "ui/add_version_to_qml_context.hpp"
#include "ui/frame_provider_google_video_device.hpp"
#include "ui/frame_provider_google_video_frame.hpp"
#include "utils_model.hpp"
#include "video_layout/video_layout.hpp"
#include <QApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QResource>

using namespace client;

ui_module::ui_module(
    std::shared_ptr<utils::executor_module> executor_module,
    std::shared_ptr<rtc::google::module> rtc_module,
    std::shared_ptr<audio_module> client_audio_module,
    std::shared_ptr<audio_settings_module> client_audio_settings_module,
    std::shared_ptr<mute_deaf_communicator> mute_deaf_communicator_,
    std::shared_ptr<video_module> client_video_module,
    std::shared_ptr<desktop_sharing_module> client_desktop_sharing_module,
    std::shared_ptr<session_module> client_session_module, int &argc,
    char *argv[])
    : executor_module{executor_module}, rtc_module{rtc_module},
      client_audio_module{client_audio_module},
      client_audio_settings_module{client_audio_settings_module},
      mute_deaf_communicator_{mute_deaf_communicator_},
      client_video_module{client_video_module},
      client_desktop_sharing_module{client_desktop_sharing_module},
      client_session_module{client_session_module} {
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  // do not use QGuiApplication. Charts needs the widgets QApplication
  // https://doc.qt.io/qt-5/qtcharts-qmlmodule.html
  app = std::make_shared<QApplication>(argc, argv);
  app->setOrganizationName("Freckled OG");
  app->setOrganizationDomain("freckled.dev");
  app->setApplicationName("Fubble");

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
  app->setWindowIcon(QIcon(":/images/fubble.svg"));
  // we are regestering with full namespace. so use full namespace in signals
  // and properties
  qRegisterMetaType<ui::frame_provider_google_video_source *>();
  qRegisterMetaType<ui::frame_provider_google_video_device *>();

  // https://doc.qt.io/qt-5/qtqml-cppintegration-overview.html#choosing-the-correct-integration-method-between-c-and-qml
  qmlRegisterUncreatableType<room_model>("io.fubble", 1, 0, "RoomModel",
                                         "can't instance room_model");
  qmlRegisterUncreatableType<participant_model>(
      "io.fubble", 1, 0, "ParticipantModel",
      "can't instance participant_model");
  qmlRegisterUncreatableType<join_model>("io.fubble", 1, 0, "JoinModel",
                                         "can't instance join_model");
  qmlRegisterUncreatableType<share_desktop_model>(
      "io.fubble", 1, 0, "ShareDesktopModel",
      "can't instance share_desktop_model");
  qmlRegisterUncreatableType<utils_model>("io.fubble", 1, 0, "UtilsModel",
                                          "can't instance utils_model");
  qmlRegisterUncreatableType<error_model>("io.fubble", 1, 0, "ErrorModel",
                                          "can't instance error_model");
  qmlRegisterUncreatableType<leave_model>("io.fubble", 1, 0, "LeaveModel",
                                          "can't instance leave_model");
  qmlRegisterUncreatableType<rooms_model>("io.fubble", 1, 0, "RoomsModel",
                                          "can't instance rooms_model");
  qmlRegisterUncreatableType<language_settings_model>(
      "io.fubble", 1, 0, "LanguageSettingsModel", "can't instance leave_model");
  qmlRegisterUncreatableType<chat_model>("io.fubble", 1, 0, "ChatModel",
                                         "can't instance chat_model");
  qmlRegisterUncreatableType<chat_messages_model>(
      "io.fubble", 1, 0, "ChatMessagesModel",
      "can't instance chat_messages_model");
  qmlRegisterUncreatableType<audio_video_settings_model>(
      "io.fubble", 1, 0, "AudioVideoSettingsModel",
      "can't instance audio_video_settings_model");
  qmlRegisterUncreatableType<own_media_model>(
      "io.fubble", 1, 0, "OwnMediaModel", "can't instance own_media_model");
  qmlRegisterUncreatableType<devices_model>("io.fubble", 1, 0, "DevicesModel",
                                            "can't instance devices_model");
  qmlRegisterType<video_layout>("io.fubble", 1, 0, "VideoLayout");
  engine = std::make_shared<QQmlApplicationEngine>();
  model_creator_ = std::make_shared<model_creator>(
      *client_audio_module->get_audio_level_calculator_factory(),
      *client_audio_settings_module->get_audio_device_settings(),
      *client_video_module->get_video_settings(),
      *client_audio_module->get_own_audio_information(),
      *client_audio_settings_module->get_audio_tracks_volume(),
      mute_deaf_communicator_);
  error_model_ = std::make_shared<error_model>();
  utils_model_ = std::make_shared<utils_model>();
  join_model_ = std::make_shared<join_model>(
      *model_creator_, *error_model_, *client_session_module->get_joiner());
  share_desktop_model_ = std::make_shared<share_desktop_model>(
      client_desktop_sharing_module->get_desktop_sharing(),
      client_desktop_sharing_module->get_desktop_sharing_previews());
  leave_model_ =
      std::make_shared<leave_model>(*client_session_module->get_leaver());
  own_media_model_ = std::make_shared<own_media_model>(
      *client_audio_settings_module->get_audio_device_settings(),
      *client_video_module->get_video_settings(),
      *client_audio_settings_module->get_own_microphone_tester(),
      *client_audio_settings_module->get_audio_tracks_volume(),
      *client_audio_module->get_own_audio_information(),
      *client_audio_settings_module->get_own_audio_test_information(),
      *client_session_module->get_own_media());
  audio_video_settings_model_ = std::make_shared<audio_video_settings_model>(
      rtc_module->get_factory()->get_audio_devices(),
      *client_video_module->get_enumerator(),
      *rtc_module->get_video_device_creator(),
      *client_audio_settings_module->get_audio_device_settings(),
      *client_video_module->get_video_settings(), *error_model_,
      executor_module->get_timer_factory());
  rooms_model_ = std::make_shared<rooms_model>(
      client_session_module->get_rooms(), model_creator_);
  language_settings_model_ = std::make_shared<language_settings_model>(*engine);

  //  works from 5.14 onwards
  // engine.setInitialProperties(...)
  //  setContextProperty sets it globaly not as property of the window
  QQmlContext *qml_context = engine->rootContext();
  qml_context->setContextProperty("joinModelFromCpp", join_model_.get());
  qml_context->setContextProperty("errorModelFromCpp", error_model_.get());
  qml_context->setContextProperty("ownMediaModelFromCpp",
                                  own_media_model_.get());
  qml_context->setContextProperty("utilsModelFromCpp", utils_model_.get());
  qml_context->setContextProperty("leaveModelFromCpp", leave_model_.get());
  qml_context->setContextProperty("audioVideoModelFromCpp",
                                  audio_video_settings_model_.get());
  qml_context->setContextProperty("shareDesktopModelFromCpp",
                                  share_desktop_model_.get());
  qml_context->setContextProperty("roomsModelFromCpp", rooms_model_.get());
  qml_context->setContextProperty("languageModelFromCpp",
                                  language_settings_model_.get());
  ui_add_version_to_qml_context =
      std::make_shared<ui::add_version_to_qml_context>(*qml_context);
  //  seems not to do it either
  // QVariant property{qMetaTypeId<client::join_model *>(), &join_model};
  // engine.setProperty("joinModel", property);

  const QUrl url(QStringLiteral("qrc:/FubbleApplication.qml"));
  BOOST_LOG_SEV(logger, logging::severity::debug) << "loading qml";
  engine->load(url);
  BOOST_LOG_SEV(logger, logging::severity::debug) << "loaded qml";
}

int ui_module::exec() { return app->exec(); }
