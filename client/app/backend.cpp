#include "backend.hpp"
#include <QStandardPaths>

using namespace client;

namespace {
QString config_file() {
  auto config_path =
      QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
  return config_path + "/fubble-join.ini";
}
} // namespace

backend::backend() : settings(config_file(), QSettings::IniFormat) {
  name = settings.value("name").toString();
  room = settings.value("room").toString();
}

backend::~backend() = default;

void backend::join(const QString &room, const QString &name) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "join(), name:" << name.toStdString()
      << ", room:" << room.toStdString();
  settings.setValue("name", name);
  settings.setValue("room", room);
}

