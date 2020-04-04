#include "room.hpp"
#include "participant.hpp"
#include "participant_creator.hpp"
#include "session/client.hpp"
#include "session/room.hpp"

using namespace client;

room::room(std::unique_ptr<participant_creator> participant_creator_parameter,
           std::unique_ptr<session::client> client_parameter,
           std::unique_ptr<session::room> room_parameter)
    : participant_creator_(std::move(participant_creator_parameter)),
      client_(std::move(client_parameter)), room_(std::move(room_parameter)) {
  on_session_participant_joins(room_->get_participants());
  room_->on_joins.connect(
      [this](const auto joins) { on_session_participant_joins(joins); });
  room_->on_updates.connect(
      [this](const auto updates) { on_session_participant_updates(updates); });
  room_->on_leaves.connect(
      [this](const auto leaves) { on_session_participant_leaves(leaves); });
}

room::~room() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "destructor";
}

std::vector<participant *> room::get_participants() const {
  std::vector<participant *> result;
  std::transform(participants_.cbegin(), participants_.cend(),
                 std::back_inserter(result), [](const auto &item) {
                   BOOST_ASSERT(item);
                   return item.get();
                 });
  return result;
}

std::string room::get_name() const { return room_->get_name(); }

std::string room::get_own_id() const { return client_->get_id(); }

void room::on_session_participant_joins(
    const std::vector<session::participant> &joins) {
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "on_session_participant_joins, count:" << joins.size();
  std::vector<participant *> signal_joins;
  for (auto join : joins) {
    BOOST_ASSERT(find(join.id) == participants_.end());
    auto participant = participant_creator_->create(join);
    signal_joins.emplace_back(participant.get());
    participants_.emplace_back(std::move(participant));
  }
  on_participants_join(signal_joins);
}
void room::on_session_participant_updates(
    const std::vector<session::participant> &updates) {
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "on_session_participant_updates, count:" << updates.size();
  std::vector<participant *> signal_updates;
  for (const auto &update : updates) {
    auto found = find(update.id);
    if (found == participants_.end()) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << "participant not in list for update, id:" << update.id;
      BOOST_ASSERT(false);
      continue;
    }
    (*found)->update(update);
    signal_updates.emplace_back(found->get());
  }
  on_participants_updated(signal_updates);
}
void room::on_session_participant_leaves(
    const std::vector<std::string> &leaves) {
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "on_session_participant_leaves, count:" << leaves.size();
  for (const auto &leave : leaves) {
    auto found = find(leave);
    if (found == participants_.end()) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << "participant not in list for removal, id:" << leave;
      BOOST_ASSERT(false);
      continue;
    }
    participants_.erase(found);
  }
  on_participants_left(leaves);
}

room::participants::iterator room::find(const std::string &id) {
  return std::find_if(participants_.begin(), participants_.end(),
                      [&](auto &check) { return check->get_id() == id; });
}
