#include "room.hpp"
#include "client.hpp"
#include "matrix/user.hpp"
#include <fmt/format.h>

using namespace session;

room::room(matrix::room &room_) : room_(room_) {
  signal_connections.emplace_back(
      room_.on_join.connect([this](const auto &user) { on_join(user); }));
}

const room::participants &room::get_participants() const {
  return participants_;
}

std::string room::get_name() const { return "TODO room::get_name"; }

std::string room::own_id() const { return "TODO room::own_id"; }

void room::on_join(const matrix::user &user) {
  participant add;
  add.id = user.get_id();
  add.name = user.get_display_name();
  BOOST_ASSERT(participants_.end() == find_iterator(add.id));
  participants_.push_back(add);
  on_joins({add});
}

room::participants::iterator room::find_iterator(const std::string &user_id) {
  return std::find_if(participants_.begin(), participants_.end(),
                      [&](const auto &check) { return user_id == check.id; });
}
