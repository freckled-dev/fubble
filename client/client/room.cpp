#include "room.hpp"
#include "participant.hpp"
#include "participant_creator.hpp"
#include "session/client.hpp"
#include "session/room.hpp"

using namespace client;

room::room(std::unique_ptr<participant_creator> participant_creator_,
           std::unique_ptr<session::client> client_,
           std::unique_ptr<session::room> room_)
    : participant_creator_(std::move(participant_creator_)),
      client_(std::move(client_)), room_(std::move(room_)) {
  room_->on_joins.connect(
      [this](const auto joins) { on_session_participant_joins(joins); });
  room_->on_leaves.connect(
      [this](const auto leaves) { on_session_participant_leaves(leaves); });
}

room::~room() = default;

const std::string &room::get_name() const { return room_->get_name(); }

void room::on_session_participant_joins(
    const std::vector<session::participant> &joins) {
  std::vector<participant *> signal_joins;
  for (auto join : joins) {
    auto participant = participant_creator_->create(join);
    signal_joins.emplace_back(participant.get());
    participants.emplace_back(std::move(participant));
  }
  on_participants_join(signal_joins);
}
void room::on_session_participant_leaves(
    const std::vector<std::string> &leaves) {
  (void)leaves;
}
