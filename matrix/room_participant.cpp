#include "room_participant.hpp"
#include "user.hpp"
#include <fmt/chrono.h>
#include <fmt/format.h>

using namespace matrix;

std::ostream &matrix::operator<<(std::ostream &out, const join_state print) {
  switch (print) {
  case join_state::invite:
    return out << "invite";
  case join_state::join:
    return out << "join";
  case join_state::leave:
    return out << "leave";
  }
  BOOST_ASSERT(false);
  return out << "undefined";
}

room_participant::room_participant(user &user_, const join_state join_state_,
                                   const timestamp_type &join_state_timestamp)
    : user_(user_), logger{fmt::format("room_participant {}", user_.get_id())},
      join_state_timestamp(join_state_timestamp), join_state_(join_state_) {}

user &room_participant::get_user() const { return user_; }

join_state room_participant::get_join_state() const { return join_state_; }

void room_participant::set_join_state(const join_state new_,
                                      const timestamp_type &timestamp) {
  if (join_state_ == new_)
    return;
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", current:" << join_state_ << ", new:" << new_;
  if (timestamp < join_state_timestamp) {
    BOOST_LOG_SEV(logger, logging::severity::info) << fmt::format(
        "ignoring join_state due to the timestamps: parameter:{}, member:{}",
        timestamp.time_since_epoch(), join_state_timestamp.time_since_epoch());
    return;
  }

  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("setting join_state to timestamp:{} state:{}",
                     timestamp.time_since_epoch(), new_);
  join_state_timestamp = timestamp;
  join_state_ = new_;
  on_join_state_changed();
}

std::string room_participant::get_id() const { return user_.get_id(); }
