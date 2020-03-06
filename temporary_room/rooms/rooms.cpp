#include "rooms.hpp"

using namespace temporary_room::rooms;

rooms::rooms(room_factory &factory) : factory(factory) {}

rooms::~rooms() = default;

void rooms::add_participant(participant &add, const room_name &name) {
  auto found = rooms_.find(name);
  if (found != rooms_.cend()) {
    auto &participants = found->second.participants;
    BOOST_ASSERT(std::find_if(participants.cbegin(), participants.cend(),
                              [&](auto check) { return check == &add; }) ==
                 participants.cend());
    participants.push_back(&add);
    auto &id = found->second.id;
    if (id)
      add.set_room(id.value());
    return;
  }
  room room_;
  room_.participants.push_back(&add);
  rooms_[name] = std::move(room_);
  create(name);
}

void rooms::create(const room_name &name) {
  factory.create().then(
      executor, [this, name](auto result) { on_created(name, result); });
}

void rooms::on_created(const room_name &name, boost::future<room_id> &result) {
  try {
    const auto id = result.get();
    BOOST_LOG_SEV(logger, logging::severity::info) << "created room, id:" << id;
    auto found = rooms_.find(name);
    if (found == rooms_.cend()) {
      BOOST_LOG_SEV(logger, logging::severity::warning)
          << "could not find room by name. May happen if all participants left "
             "before room got created, name:"
          << name;
      destroy(id);
      return;
    }
    for (auto &participant_ : found->second.participants)
      participant_->set_room(id);
  } catch (const std::runtime_error &error) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "failed to create room, message:" << error.what();
  }
}

void rooms::destroy(const room_id &id) { factory.destroy(id); }
