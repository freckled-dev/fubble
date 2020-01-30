#include "participant_creator.hpp"
#include "peer_creator.hpp"

using namespace client;

participant_creator::participant_creator(peer_creator &peer_creator_,
                                         const std::string &own_id)
    : peer_creator_(peer_creator_), own_id(own_id) {}

std::unique_ptr<participant>
participant_creator::create(const session::participant &session_information) {
  if (session_information.id == own_id) {
    // TODO do own participant
    return nullptr;
  }
  auto peer = peer_creator_.create();
  return std::make_unique<participant>(std::move(peer), session_information);
}
