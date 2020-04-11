#include "room_model.hpp"
#include "client/room.hpp"
#include "session/room.hpp"

using namespace client;

room_model::room_model(const std::shared_ptr<room> &room_, QObject *parent)
    : QObject(parent), room_(room_) {
  participants = new participants_model(*room_, this);
  signal_connections.emplace_back(
      room_->on_name_changed.connect([this](const auto &) { set_name(); }));
  set_name();
}

void room_model::set_name() {
  auto name_ = room_->get_name();
  name = QString::fromStdString(name_);
  name_changed(name);
}
