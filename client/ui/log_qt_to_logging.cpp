#include "log_qt_to_logging.hpp"
#include <QString>
#include <QtGlobal>

using namespace client::ui;

namespace {
logging::logger *logger{};
void on_message(QtMsgType type, const QMessageLogContext &context,
                const QString &msg) {
  (void)context;
  logging::severity severity{logging::severity::trace};
  switch (type) {
  case QtDebugMsg:
    severity = logging::severity::debug;
    break;
  case QtInfoMsg:
    severity = logging::severity::info;
    break;
  case QtWarningMsg:
    severity = logging::severity::warning;
    break;
  case QtCriticalMsg:
  case QtFatalMsg:
    severity = logging::severity::error;
    break;
  default:
    break;
  }
  BOOST_LOG_SEV(*logger, severity) << msg.toStdString();
}
} // namespace

log_qt_to_logging::log_qt_to_logging() : logger{"qt"} {
  BOOST_ASSERT(!::logger);
  ::logger = &logger;
  qInstallMessageHandler(on_message);
}

log_qt_to_logging::~log_qt_to_logging() {
  qInstallMessageHandler(0);
  ::logger = nullptr;
}
