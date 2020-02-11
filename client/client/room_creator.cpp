#include "room_creator.hpp"
#include "room.hpp"
#include "session/client.hpp"
#include "session/room.hpp"

using namespace client;

room_creator::room_creator(
    participant_creator_creator &participant_creator_creator_)
    : participant_creator_creator_(participant_creator_creator_) {}

std::unique_ptr<room>
room_creator::create(std::unique_ptr<session::client> client_,
                     std::unique_ptr<session::room> room_) {
  auto own_id = room_->own_id();
  BOOST_ASSERT(!own_id.empty());
  auto participant_creator_ = participant_creator_creator_.create(own_id);
  return std::make_unique<room>(std::move(participant_creator_),
                                std::move(client_), std::move(room_));
}
