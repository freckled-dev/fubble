#include "join_model.hpp"
#include "client/joiner.hpp"
#include "client/own_media.hpp"
#include "client/room.hpp"
#include "client/ui/frame_provider_google_video_frame.hpp"
#include "error_model.hpp"
#include "model_creator.hpp"
#include "room_model.hpp"

using namespace client;

join_model::join_model(model_creator &model_factory, error_model &error_model_,
                       joiner &joiner_, own_media &own_media_)
    : model_factory(model_factory), error_model_(error_model_),
      joiner_(joiner_), own_media_(own_media_) {
  update_video_preview();
  own_media_.on_video_added.connect([this](auto &) { update_video_preview(); });
  own_media_.on_video_removed.connect(
      [this](auto &) { update_video_preview(); });
}

join_model::~join_model() = default;

void join_model::join(const QString &room, const QString &name) {
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
  BOOST_LOG_SEV(logger, logging::severity::debug) << "on_joined";
  try {
    auto got_room = room_.get();
    auto room_model_ = model_factory.create_room_model(got_room, this);
    joined(room_model_);
  } catch (const std::exception &error) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << "could not join room, what:" << error.what();
    error_model_.set_error(error_model::type::could_not_connect_to_backend,
                           error.what());
    join_failed();
  }
}

void join_model::update_video_preview() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  if (video != nullptr) {
    video->deleteLater();
    video = nullptr;
    video_changed(video);
  }

  auto own_videos = own_media_.get_videos();
  if (own_videos.empty()) {
    video_available = false;
    video_available_changed(video_available);
    return;
  }

  video_available = true;
  video_available_changed(video_available);
  BOOST_ASSERT(own_videos.size() == 1);
  auto own_video = own_videos.front();
  video = new ui::frame_provider_google_video_source(this);
  video->set_source(own_video);
  video_changed(video);
}
