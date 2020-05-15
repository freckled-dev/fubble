#include "room_model.hpp"
#include "client/room.hpp"

using namespace client;

room_model::room_model(const std::shared_ptr<room> &room_, QObject *parent)
    : QObject(parent), room_(room_) {
  participants = new participants_model(*room_, this);
  participants_with_video = new participants_with_video_model(*room_, this);
  chat = new chat_model(*room_, this);
  own_participant = participants->get_own().value_or(nullptr);
  signal_connections.emplace_back(
      participants->on_own_changed.connect([this]() { set_own(); }));
  signal_connections.emplace_back(
      room_->on_name_changed.connect([this](const auto &) { set_name(); }));
  set_name();
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
