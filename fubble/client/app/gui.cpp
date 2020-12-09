#include "core_module.hpp"
#include "fubble/client/audio_module.hpp"
#include "fubble/client/audio_settings_module.hpp"
#include "fubble/client/crash_catcher.hpp"
#include "fubble/client/mute_deaf_communicator.hpp"
#include "fubble/client/own_media.hpp"
#include "fubble/client/video_module.hpp"
#include "fubble/utils/logging/logger.hpp"
#include "fubble/utils/version.hpp"
#include "gui_options.hpp"
#include "log_module.hpp"
#include "poll_asio_by_qt.hpp"
#include "ui/add_version_to_qml_context.hpp"
#include "ui_module.hpp"

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

  client::core_module::config core_config{
      config.general_.host, config.general_.service, config.general_.use_ssl};
  auto core_module_ = std::make_shared<client::core_module>(core_config);

  std::shared_ptr<client::crash_catcher> crash_catcher;
  if (config.general_.use_crash_catcher)
    crash_catcher = core_module_->get_crash_catcher();

  // audio
  BOOST_LOG_SEV(logger, logging::severity::debug) << "setting up client_audio";
  client::audio_module::config client_audio_config;
  std::shared_ptr<client::audio_module> client_audio_module =
      std::make_shared<client::audio_module>(
          core_module_->get_utils_executor_module(),
          core_module_->get_rtc_module(), client_audio_config);
  // TODO refactor #355
  core_module_->get_session_module()->get_own_media()->set_own_audio_track(
      client_audio_module->get_own_audio_track());

  // audio settings // TODO merge into audio_module
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "setting up audio_settings";
  client::audio_settings_module::config client_audio_settings_config;
  std::shared_ptr<client::audio_settings_module> client_audio_settings_module =
      std::make_shared<client::audio_settings_module>(
          core_module_->get_utils_executor_module(),
          core_module_->get_rtc_module(), client_audio_module,
          core_module_->get_session_module(), client_audio_settings_config);

  // audio communicator
  std::shared_ptr<client::mute_deaf_communicator> mute_deaf_communicator_ =
      client::mute_deaf_communicator::create(
          core_module_->get_session_module()->get_rooms(),
          client_audio_settings_module->get_audio_tracks_volume());

  // video
  BOOST_LOG_SEV(logger, logging::severity::trace) << "setting up video device";
  client::video_module::config client_video_config;
  client_video_config.enabled = config.general_.video_support;
  std::shared_ptr<client::video_module> client_video_module =
      std::make_shared<client::video_module>(
          core_module_->get_utils_executor_module(),
          core_module_->get_rtc_module(), core_module_->get_session_module(),
          client_video_config);
  // TODO refactor #355
  core_module_->get_session_module()->get_own_media()->set_own_video(
      client_video_module->get_own_video());

  // desktop
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "setting up desktop sharing";
  auto client_desktop_sharing_module =
      std::make_shared<client::desktop_sharing_module>(
          core_module_->get_utils_executor_module(),
          core_module_->get_session_module(), client_video_module);
  // TODO refactor #355
  core_module_->get_session_module()->get_own_media()->set_desktop_sharing(
      client_desktop_sharing_module->get_desktop_sharing());

  // ui
  BOOST_LOG_SEV(logger, logging::severity::debug) << "setting up qt";
  auto client_ui_module = std::make_shared<client::ui_module>(
      core_module_->get_utils_executor_module(), core_module_->get_rtc_module(),
      client_audio_module, client_audio_settings_module,
      mute_deaf_communicator_, client_video_module,
      client_desktop_sharing_module, core_module_->get_session_module(), argc,
      argv);
  auto context = core_module_->get_utils_executor_module()->get_io_context();
  client::poll_asio_by_qt asio_poller{*context};
  asio_poller.run();
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "setting up all up is done. Executing the application";
  auto result = client_ui_module->exec();
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "application stopped, result:" << result;
  context->stop();
  return result;
}
