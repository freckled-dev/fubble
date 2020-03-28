#include "rooms.hpp"
#include <fmt/format.h>

using namespace temporary_room::rooms;

rooms::rooms(room_factory &factory) : factory(factory) {}

rooms::~rooms() = default;

boost::future<room_id> rooms::get_or_create_room_id(const room_name &name) {
  auto found = rooms_.find(name);
  if (found != rooms_.cend()) {
    if (found->second.room_) {
      const auto id = found->second.room_->get_room_id();
      return boost::make_ready_future(id);
    }
    auto &participants = found->second.participants;
    auto add = std::make_unique<participant>();
    auto result = add->promise->get_future();
    participants.push_back(std::move(add));
    return result;
  }
  room_adapter room_;
  auto add = std::make_unique<participant>();
  auto result = add->promise->get_future();
  room_.participants.push_back(std::move(add));
  rooms_[name] = std::move(room_);
  create(name);
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
    for (auto &participant_ : found->second.participants)
      participant_->set_room(id);
    found->second.participants.clear();
  } catch (const std::runtime_error &error) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "failed to create room, message:" << error.what();
    auto found = rooms_.find(name);
    BOOST_ASSERT(found != rooms_.cend());
    if (found == rooms_.cend())
      return;
    for (auto &participant_ : found->second.participants)
      participant_->set_error(error);
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
  rooms_.erase(found);
}
