#include "join_model.hpp"
#include "joiner.hpp"
#include "room.hpp"
#include "room_model.hpp"
#include <QStandardPaths>

using namespace client;

namespace {
QString config_file() {
  auto config_path =
      QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
  return config_path + "/fubble-join.ini";
}
} // namespace

join_model::join_model(joiner &joiner_)
    : joiner_(joiner_), settings(config_file(), QSettings::IniFormat) {
  name = settings.value("name").toString();
  room = settings.value("room").toString();
}

join_model::~join_model() = default;

void join_model::join(const QString &room, const QString &name) {
  settings.setValue("name", name);
  settings.setValue("room", room);
  joiner::parameters parameters;
  parameters.room = room.toStdString();
  parameters.name = name.toStdString();
  joiner_.join(parameters).then(qt_executor, [this](auto room) {
    on_joined(std::move(room));
  });
}

void join_model::on_joined(boost::future<std::shared_ptr<class room>> room_) {
  if (room_.has_exception()) {
    // TODO do a proper error gui message
    BOOST_LOG_SEV(logger, logging::severity::warning) << "could not join room";
    return;
  }
  auto room_model_ = new room_model(room_.get(), this);
  joined(room_model_);
}
