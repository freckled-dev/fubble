#include "room_model.hpp"
#include "client/room.hpp"
#include "model_creator.hpp"
#include "participants_model.hpp"
#include "participants_with_video_model.hpp"

using namespace client;

room_model::room_model(model_creator &model_creator_,
                       const std::shared_ptr<room> &room_, QObject *parent)
    : QObject(parent), model_creator_(model_creator_), room_(room_) {
  participants = model_creator_.create_participants_model(*room_, this);
  participants_with_video = new participants_with_video_model(*participants);
  chat = new chat_model(*room_, this);
  own_participant = participants->get_own().value_or(nullptr);
  signal_connections.emplace_back(
      participants->on_own_changed.connect([this]() { set_own(); }));
  signal_connections.emplace_back(
      room_->on_name_changed.connect([this](const auto &) { set_name(); }));
  connect(participants_with_video, &participants_with_video_model::rowsInserted,
          this, &room_model::recalculate_video_available);
  connect(participants_with_video, &participants_with_video_model::rowsRemoved,
          this, &room_model::recalculate_video_available);
  recalculate_video_available();
  set_name();
}

void room_model::recalculate_video_available() {
  auto has_videos = participants_with_video->rowCount() != 0;
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "recalculate_video_available has_videos:" << has_videos
      << ", videos_available:" << videos_available;
  if (has_videos == videos_available)
    return;
  videos_available = has_videos;
  videos_available_changed(videos_available);
}

void room_model::raise_new_participants_count() {
  ++new_participants_count;
  new_participants_count_changed(new_participants_count);
}

void room_model::set_name() {
  auto name_ = room_->get_name();
  name = QString::fromStdString(name_);
  name_changed(name);
}

void room_model::set_own() {
  auto own = participants->get_own();
  BOOST_ASSERT(own);
  own_participant = own.value();
  own_participant_changed(own_participant);
}

void room_model::resetNewParticipants() {
  new_participants_count = 0;
  new_participants_count_changed(new_participants_count);
}
