#include "room_creator.hpp"
#include "fubble/matrix/client.hpp"
#include "fubble/matrix/room.hpp"
#include "room.hpp"

using namespace client;

room_creator::room_creator(
    participant_creator_creator &participant_creator_creator_)
    : participant_creator_creator_(participant_creator_creator_) {}

std::unique_ptr<room>
room_creator::create(std::shared_ptr<matrix::client> client_,
                     matrix::room &room_) {
  auto own_id = client_->get_user_id();
  BOOST_ASSERT(!own_id.empty());
  auto participant_creator_ = participant_creator_creator_.create(own_id);
  return std::make_unique<room>(std::move(participant_creator_),
                                std::move(client_), room_);
}
