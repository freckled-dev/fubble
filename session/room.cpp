#include "room.hpp"
#include "client.hpp"
#include <fmt/format.h>

using namespace session;

room::room(matrix::room &room_) : room_(room_) {}

const room::participants &room::get_participants() const {
  return participants_;
}

std::string room::get_name() const { return "TODO room::get_name"; }

std::string room::own_id() const { return "TODO room::own_id"; }
