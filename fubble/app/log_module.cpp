#include "log_module.hpp"
#include "fubble/client/ui/log_qt_to_logging.hpp"
#include "fubble/rtc/google/log_webrtc_to_logging.hpp"
#include "fubble/utils/logging/initialser.hpp"

using namespace client;

log_module::log_module(const gui_config config) {
  logging::add_console_log(config.general_.log_severity);
  logging::add_file_log(config.general_.log_severity);
  client::ui::log_qt_to_logging qt_logger;
  rtc::google::log_webrtc_to_logging webrtc_logger;
  webrtc_logger.set_enabled(config.general_.log_webrtc);
}