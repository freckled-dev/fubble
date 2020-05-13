#include "user.hpp"
#include <room_participant.hpp>

using namespace matrix;

room_participant::room_participant(user &user_, const join_state join_state_)
    : user_(user_), join_state_(join_state_) {}

user &room_participant::get_user() const { return user_; }

join_state room_participant::get_join_state() const { return join_state_; }

void room_participant::set_join_state(const join_state new_) {
  if (join_state_ == new_)
    return;
  join_state_ = new_;
  on_join_state_changed();
}

std::string room_participant::get_id() const { return user_.get_id(); }
