#include "room.hpp"
#include "client.hpp"
#include "matrix/user.hpp"
#include <fmt/format.h>

using namespace session;

room::room(matrix::room &room_) : room_(room_) {
  signal_connections.emplace_back(
      room_.on_join.connect([this](const auto &user) { on_join(user); }));
  signal_connections.emplace_back(
      room_.on_leave.connect([this](const auto &id) { on_leave(id); }));
  signal_connections.emplace_back(room_.on_name_changed.connect(
      [this](const auto &id) { on_name_changed(id); }));
}

const room::participants &room::get_participants() const {
  return participants_;
}

std::string room::get_name() const { return room_.get_name(); }

std::string room::get_id() const { return room_.get_id(); }

void room::on_join(const matrix::user &user) {
  participant add;
  add.id = user.get_id();
  add.name = user.get_display_name();
  BOOST_ASSERT(participants_.end() == find_iterator(add.id));
  participants_.push_back(add);
  on_joins({add});
}

void room::on_leave(const std::string &user_id) {
  auto found = find_iterator(user_id);
  if (found == participants_.cend()) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "on_leave, could not find user by id:" << user_id;
    BOOST_ASSERT(false);
    return;
  }
  participants_.erase(found);
  on_leaves({user_id});
}

room::participants::iterator room::find_iterator(const std::string &user_id) {
  return std::find_if(participants_.begin(), participants_.end(),
                      [&](const auto &check) { return user_id == check.id; });
}
