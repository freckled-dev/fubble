#include "log_module.hpp"
#include "fubble/rtc/google/log_webrtc_to_logging.hpp"
#include "fubble/utils/logging/initialser.hpp"

using namespace client;

log_module::log_module(const config config_) {
  logging::add_console_log(config_.severity);
  logging::add_file_log(config_.severity);
  rtc::google::log_webrtc_to_logging webrtc_logger;
  webrtc_logger.set_enabled(config_.webrtc);
}
