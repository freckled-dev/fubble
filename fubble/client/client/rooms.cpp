#include "rooms.hpp"
#include "room.hpp"

using namespace client;

rooms::rooms() {}
rooms::~rooms() {}

void rooms::set(const std::shared_ptr<room> &set_) {
  room_ = set_;
  on_set();
}

std::shared_ptr<room> rooms::get() const { return room_; }
