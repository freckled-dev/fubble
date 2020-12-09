#include "rooms_model.hpp"
#include "fubble/client/room.hpp"
#include "fubble/client/rooms.hpp"
#include "model_creator.hpp"
#include "room_model.hpp"

using namespace client;

rooms_model::rooms_model(std::shared_ptr<rooms> rooms_,
                         std::shared_ptr<model_creator> model_creator_,
                         QObject *parent)
    : QObject(parent), rooms_{rooms_}, model_creator_{model_creator_} {
  signal_connections.push_back(
      rooms_->on_set.connect([this]() { on_room_set(); }));
}

void rooms_model::on_room_set() {
  std::shared_ptr<room> set = rooms_->get();
  BOOST_ASSERT((set == nullptr && room_ != nullptr) ||
               (set != nullptr && room_ == nullptr));
  if (set) {
    room_ = model_creator_->create_room_model(set, this);
  } else {
    room_->deleteLater();
    room_ = nullptr;
  }
  room_changed(room_);
}
