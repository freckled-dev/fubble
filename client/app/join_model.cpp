#include "join_model.hpp"
#include "client/joiner.hpp"
#include "client/own_media.hpp"
#include "client/room.hpp"
#include "model_creator.hpp"
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

join_model::join_model(model_creator &model_factory, joiner &joiner_,
                       own_media &own_media_)
    : model_factory(model_factory), joiner_(joiner_), own_media_(own_media_),
      settings(config_file(), QSettings::IniFormat) {
  name = settings.value("name").toString();
  room = settings.value("room").toString();
  auto own_videos = own_media_.get_videos();
  if (own_videos.empty())
    return;
  BOOST_ASSERT(own_videos.size() == 1);
  auto own_video = own_videos.front();
  video = new ui::frame_provider_google_video_source(this);
  video->set_source(own_video);
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

ui::frame_provider_google_video_source *join_model::get_video() const {
  return video;
}

void join_model::on_joined(boost::future<std::shared_ptr<class room>> room_) {
  if (room_.has_exception()) {
    // TODO do a proper error gui message
    BOOST_LOG_SEV(logger, logging::severity::warning) << "could not join room";
    return;
  }
  auto room_model_ = model_factory.create_room_model(room_.get(), this);
  joined(room_model_);
}
