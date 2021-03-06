#include "log_webrtc_to_logging.hpp"
#include "fubble/utils/logging/logger.hpp"
#include <rtc_base/logging.h>

using namespace rtc::google;

class log_webrtc_to_logging::sink : public rtc::LogSink {
public:
  void OnLogMessage(const std::string &message,
                    LoggingSeverity severity_to_cast) override {
    if (!enabled)
      return;
    logging::severity severity{logging::severity::debug};
    switch (severity_to_cast) {
    case LoggingSeverity::LS_INFO:
    case LoggingSeverity::LS_NONE:
      severity = logging::severity::debug;
      break;
    case LoggingSeverity::LS_ERROR:
      severity = logging::severity::error;
      break;
    case LoggingSeverity::LS_WARNING:
      severity = logging::severity::warning;
      break;
    case LoggingSeverity::LS_VERBOSE:
      severity = logging::severity::trace;
      break;
    default:
      break;
    }
    // remove '\n' from end
    BOOST_LOG_SEV(logger, severity) << message.substr(0, message.size() - 1);
  }
  void OnLogMessage(const std::string &message) override {
    BOOST_LOG_SEV(logger, logging::severity::debug) << message;
  }
  void set_enabled(bool enabled_) { enabled = enabled_; }

protected:
  logging::logger logger{"google_webrtc"};
  bool enabled;
};

log_webrtc_to_logging::log_webrtc_to_logging() {
  rtc::LogMessage::LogToDebug(rtc::LoggingSeverity::LS_NONE);
  sink_ = new sink();
  rtc::LogMessage::AddLogToStream(sink_, rtc::LoggingSeverity::LS_VERBOSE);
}

log_webrtc_to_logging::~log_webrtc_to_logging() {
  rtc::LogMessage::RemoveLogToStream(sink_);
  delete sink_;
}

void log_webrtc_to_logging::set_enabled(bool enabled) {
  sink_->set_enabled(enabled);
}
