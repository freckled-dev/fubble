#include "rooms.hpp"
#include <fmt/format.h>

using namespace temporary_room::rooms;

rooms::rooms(room_factory &factory) : factory(factory) {}

rooms::~rooms() = default;

boost::future<room_id> rooms::get_or_create_room_id(const room_name &name,
                                                    const user_id &user_id_) {
  auto add = std::make_shared<participant>(user_id_);
  auto result = add->promise->get_future();
  auto found = rooms_.find(name);
  if (found == rooms_.cend()) {
    room_adapter room_;
    room_.participants.push_back(add);
    rooms_[name] = std::move(room_);
    create(name);
    return result;
  }
  std::optional<room_id> room_id_;
  if (found->second.room_) {
    const auto &room_ = found->second;
    invite(add, room_.room_);
  } else {
    auto &participants = found->second.participants;
    participants.push_back(add);
  }
  return result;
}

std::size_t rooms::get_room_count() { return rooms_.size(); }

void rooms::create(const room_name &name) {
  factory.create().then(
      executor, [this, name](auto result) { on_created(name, result); });
}

void rooms::on_created(const room_name &name, boost::future<room_ptr> &result) {
  try {
    auto got_room = result.get();
    const auto id = got_room->get_room_id();
    BOOST_LOG_SEV(logger, logging::severity::info) << "created room, id:" << id;
    auto found = rooms_.find(name);
    if (found == rooms_.cend()) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << "could not find room by name, name:" << name;
      BOOST_ASSERT(false);
      return;
    }
    got_room->on_participant_count_changed = [this, name](auto count) {
      on_participant_count_changed(name, count);
    };
    found->second.room_ = std::move(got_room);
    for (const auto &participant_ : found->second.participants)
      invite(participant_, found->second.room_);
    found->second.participants.clear();
  } catch (const std::exception &error) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "failed to create room, message:" << error.what();
    auto found = rooms_.find(name);
    BOOST_ASSERT(found != rooms_.cend());
    if (found == rooms_.cend())
      return;
    for (auto &participant_ : found->second.participants)
      participant_->promise->set_exception(error);
    found->second.participants.clear();
  }
}

void rooms::on_participant_count_changed(const room_name &name,
                                         const int count) {
  if (count > 1)
    return;
  BOOST_ASSERT(count == 1);
  auto found = rooms_.find(name);
  if (found == rooms_.end()) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "can't remove room because not in map, name:" << name;
    BOOST_ASSERT(false);
    return;
  }
  BOOST_ASSERT(found->second.participants.empty());
  rooms_.erase(found);
}

void rooms::invite(const std::shared_ptr<participant> participant_,
                   const room_ptr &room_) {
  participant_->room_id_ = room_->get_room_id();
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("inviting the participant '{}' to the room '{}'",
                     participant_->user_id_, participant_->room_id_);
  auto invited = room_->invite(participant_->user_id_);
  invited.then(executor, [this, participant_](auto result) {
    on_invited(participant_, std::move(result));
  });
}

void rooms::on_invited(const std::shared_ptr<participant> &participant_,
                       boost::future<void> result) {
  auto safe_promise = participant_->promise;
  if (result.has_exception())
    return safe_promise->set_exception(result.get_exception_ptr());
  safe_promise->set_value(participant_->room_id_);
}

