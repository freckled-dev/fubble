#ifndef UUID_0267F6EB_70FA_4730_85C5_B11D433A7EF4
#define UUID_0267F6EB_70FA_4730_85C5_B11D433A7EF4

#include "client/audio_module.hpp"
#include "client/audio_settings_module.hpp"
#include "client/desktop_sharing_module.hpp"
#include "client/logger.hpp"
#include "client/session_module.hpp"
#include "client/video_module.hpp"
#include "utils/export.hpp"
#include <memory>

class QApplication;
class QQmlApplicationEngine;

namespace client {
namespace ui {
class add_version_to_qml_context;
}
class audio_video_settings_model;
class error_model;
class rooms_model;
class join_model;
class language_settings_model;
class leave_model;
class model_creator;
class mute_deaf_communicator;
class own_media_model;
class share_desktop_model;
class utils_model;
class FUBBLE_PUBLIC ui_module {
public:
  ui_module(
      std::shared_ptr<utils::executor_module> executor_module,
      std::shared_ptr<rtc::google::module> rtc_module,
      std::shared_ptr<audio_module> client_audio_module,
      std::shared_ptr<audio_settings_module> client_audio_settings_module,
      std::shared_ptr<mute_deaf_communicator> mute_deaf_communicator_,
      std::shared_ptr<video_module> client_video_module,
      std::shared_ptr<desktop_sharing_module> client_desktop_sharing_module,
      std::shared_ptr<session_module> client_session_module, int &argc,
      char *argv[]);

  std::shared_ptr<QApplication> get_app();
  int exec();

protected:
  client::logger logger{"ui_module"};

  std::shared_ptr<utils::executor_module> executor_module;
  std::shared_ptr<rtc::google::module> rtc_module;
  std::shared_ptr<audio_module> client_audio_module;
  std::shared_ptr<audio_settings_module> client_audio_settings_module;
  std::shared_ptr<mute_deaf_communicator> mute_deaf_communicator_;
  std::shared_ptr<video_module> client_video_module;
  std::shared_ptr<desktop_sharing_module> client_desktop_sharing_module;
  std::shared_ptr<session_module> client_session_module;

  std::shared_ptr<QApplication> app;
  std::shared_ptr<QQmlApplicationEngine> engine;
  std::shared_ptr<model_creator> model_creator_;
  std::shared_ptr<utils_model> utils_model_;
  std::shared_ptr<join_model> join_model_;
  std::shared_ptr<rooms_model> rooms_model_;
  std::shared_ptr<language_settings_model> language_settings_model_;
  std::shared_ptr<share_desktop_model> share_desktop_model_;
  std::shared_ptr<error_model> error_model_;
  std::shared_ptr<leave_model> leave_model_;
  std::shared_ptr<own_media_model> own_media_model_;
  std::shared_ptr<audio_video_settings_model> audio_video_settings_model_;
  std::shared_ptr<ui::add_version_to_qml_context> ui_add_version_to_qml_context;
};
} // namespace client

#endif
