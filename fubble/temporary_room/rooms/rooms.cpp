#include "rooms.hpp"
#include <fmt/format.h>
#include <boost/algorithm/string.hpp>

using namespace temporary_room::rooms;

namespace {
std::string remove_whitespaces_and_lower_case(std::string input) {
  input.erase(std::remove_if(input.begin(), input.end(), ::isspace),
              input.end());
  boost::algorithm::to_lower(input);
  return input;
}
} // namespace

rooms::rooms(room_factory &factory) : factory(factory) {
  BOOST_ASSERT(!factory.on_room);
  factory.on_room = [this](auto room_) { on_room(room_); };
}

rooms::~rooms() = default;

boost::future<room_id> rooms::get_or_create_room_id(const room_name &name_fix,
                                                    const user_id &user_id_) {
  const std::string name = remove_whitespaces_and_lower_case(name_fix);
  auto add = std::make_shared<participant>(user_id_);
  auto result = add->promise->get_future();
  auto found = rooms_.find(name);
  if (found == rooms_.cend()) {
    room_adapter room_;
    room_.waiting_for_room.push_back(add);
    rooms_[name] = std::move(room_);
    create(name);
    on_room_count_changed(static_cast<int>(rooms_.size()));
    return result;
  }
  boost::optional<room_id> room_id_;
  if (found->second.room_) {
    const auto &room_ = found->second;
    invite(add, room_.room_);
  } else {
    auto &participants = found->second.waiting_for_room;
    participants.push_back(add);
  }
  return result;
}

std::size_t rooms::get_room_count() { return rooms_.size(); }

void rooms::on_room(room_ptr room_) {
  BOOST_ASSERT(room_);
  on_new_room(room_);
}

void rooms::create(const room_name &name) { factory.create(name); }

void rooms::on_new_room(room_ptr room_) {
  const auto id = room_->get_room_id();
  const auto name = room_->get_room_name();
  BOOST_ASSERT(!name.empty());
  BOOST_LOG_SEV(logger, logging::severity::info) << "created room, id:" << id;
  auto found = rooms_.find(name);
  if (found == rooms_.cend()) {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "could not find room by name, name:" << name;
    room_adapter room_adapter_;
    rooms_[name] = std::move(room_adapter_);
    found = rooms_.find(name);
  }
  BOOST_ASSERT(found != rooms_.cend());
  room_->on_empty = [this, name]() { on_empty(name); };
  found->second.room_ = room_;
  for (const auto &participant_ : found->second.waiting_for_room)
    invite(participant_, found->second.room_);
  found->second.waiting_for_room.clear();
}

void rooms::on_empty(const room_name &name) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "closing room, because it's empty";
  auto found = rooms_.find(name);
  if (found == rooms_.end()) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "can't remove room because not in map, name:" << name;
    BOOST_ASSERT(false);
    return;
  }
  BOOST_ASSERT(found->second.waiting_for_room.empty());
  rooms_.erase(found);
  on_room_count_changed(static_cast<int>(rooms_.size()));
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
