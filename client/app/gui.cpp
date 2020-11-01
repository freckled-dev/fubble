#include "client/audio_module.hpp"
#include "client/audio_settings_module.hpp"
#include "client/crash_catcher.hpp"
#include "client/mute_deaf_communicator.hpp"
#include "client/own_media.hpp"
#include "client/session_module.hpp"
#include "client/video_module.hpp"
#include "gui_options.hpp"
#include "log_module.hpp"
#include "logging/logger.hpp"
#include "matrix/module.hpp"
#include "poll_asio_by_qt.hpp"
#include "rtc/google/module.hpp"
#include "signalling/client_module.hpp"
#include "temporary_room/client_module.hpp"
#include "ui/add_version_to_qml_context.hpp"
#include "ui_module.hpp"
#include "utils/executor_module.hpp"
#include "utils/version.hpp"
#include "version/client_module.hpp"

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

  // executor
  BOOST_LOG_SEV(logger, logging::severity::debug) << "setting up executor";
  auto executor_module_ = std::make_shared<utils::executor_module>();
  boost::asio::io_context &context(*executor_module_->get_io_context());

  // http
  BOOST_LOG_SEV(logger, logging::severity::debug) << "setting up http";
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
  auto matrix_module = std::make_shared<matrix::module>(
      executor_module_, http_client_module, matrix_config);

  // temporary_room
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "setting up temporary_room";
  temporary_room::client_module::config temporary_room_config;
  temporary_room_config.host = config.general_.host;
  temporary_room_config.service = config.general_.service;
  temporary_room_config.use_ssl = config.general_.use_ssl;
  auto temporary_room_module = std::make_shared<temporary_room::client_module>(
      http_client_module, temporary_room_config);

  // version
  BOOST_LOG_SEV(logger, logging::severity::debug) << "setting up version";
  version::client_module::config version_client_config;
  version_client_config.host = config.general_.host;
  version_client_config.service = config.general_.service;
  version_client_config.use_ssl = config.general_.use_ssl;
  std::shared_ptr<version::client_module> version_client_module =
      std::make_shared<version::client_module>(http_client_module,
                                               version_client_config);

  // webrtc
  BOOST_LOG_SEV(logger, logging::severity::trace) << "setting up webrtc";
  rtc::google::settings rtc_settings;
  rtc_settings.use_ip_v6 = config.general_.use_ipv6;
  auto rtc_module =
      std::make_shared<rtc::google::module>(executor_module_, rtc_settings);

  // client
  BOOST_LOG_SEV(logger, logging::severity::debug) << "setting up client";
  client::session_module::config client_session_config;
  std::shared_ptr<client::session_module> client_session_module =
      std::make_shared<client::session_module>(
          executor_module_, matrix_module, rtc_module, signalling_module,
          temporary_room_module, version_client_module, client_session_config);

  // audio
  BOOST_LOG_SEV(logger, logging::severity::debug) << "setting up client_audio";
  client::audio_module::config client_audio_config;
  std::shared_ptr<client::audio_module> client_audio_module =
      std::make_shared<client::audio_module>(executor_module_, rtc_module,
                                             client_audio_config);
  // TODO refactor #355
  client_session_module->get_own_media()->set_own_audio_track(
      client_audio_module->get_own_audio_track());

  // audio settings // TODO merge into audio_module
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "setting up audio_settings";
  client::audio_settings_module::config client_audio_settings_config;
  std::shared_ptr<client::audio_settings_module> client_audio_settings_module =
      std::make_shared<client::audio_settings_module>(
          executor_module_, rtc_module, client_audio_module,
          client_session_module, client_audio_settings_config);

  // audio communicator
  std::shared_ptr<client::mute_deaf_communicator> mute_deaf_communicator_ =
      client::mute_deaf_communicator::create(
          client_session_module->get_rooms(),
          client_audio_settings_module->get_audio_tracks_volume());

  // video
  BOOST_LOG_SEV(logger, logging::severity::trace) << "setting up video device";
  client::video_module::config client_video_config;
  client_video_config.enabled = config.general_.video_support;
  std::shared_ptr<client::video_module> client_video_module =
      std::make_shared<client::video_module>(executor_module_, rtc_module,
                                             client_session_module,
                                             client_video_config);
  // TODO refactor #355
  client_session_module->get_own_media()->set_own_video(
      client_video_module->get_own_video());

  // desktop
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "setting up desktop sharing";
  auto client_desktop_sharing_module =
      std::make_shared<client::desktop_sharing_module>(
          executor_module_, client_session_module, client_video_module);
  // TODO refactor #355
  client_session_module->get_own_media()->set_desktop_sharing(
      client_desktop_sharing_module->get_desktop_sharing());

  // ui
  BOOST_LOG_SEV(logger, logging::severity::debug) << "setting up qt";
  auto client_ui_module = std::make_shared<client::ui_module>(
      executor_module_, rtc_module, client_audio_module,
      client_audio_settings_module, mute_deaf_communicator_,
      client_video_module, client_desktop_sharing_module, client_session_module,
      argc, argv);
  client::poll_asio_by_qt asio_poller{context};
  asio_poller.run();
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "setting up all up is done. Executing the application";
  auto result = client_ui_module->exec();
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "application stopped, result:" << result;
  context.stop();
  return result;
}
