#include "room.hpp"
#include "participant.hpp"
#include "participant_creator.hpp"
#include "session/client.hpp"
#include "session/room.hpp"

using namespace client;

room::room(boost::executor &session_thread,
           participant_creator &participant_creator_,
           std::unique_ptr<session::client> client_,
           std::unique_ptr<session::room> room_)
    : session_thread(session_thread),
      participant_creator_(participant_creator_), client_(std::move(client_)),
      room_(std::move(room_)) {
  room_->on_joins.connect(
      [this](const auto joins) { on_session_participant_joins(joins); });
  room_->on_leaves.connect(
      [this](const auto leaves) { on_session_participant_leaves(leaves); });
}

room::~room() = default;

const std::string &room::get_name() const { return room_->get_name(); }

boost::executor &room::get_session_thread() { return session_thread; }

void room::on_session_participant_joins(
    const std::vector<session::participant> &joins) {
  for (auto join : joins) {
    participant_creator_.create(join);
  }
}
void room::on_session_participant_leaves(
    const std::vector<std::string> &leaves) {
  (void)leaves;
}
