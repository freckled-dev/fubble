#include "participant_creator_creator.hpp"

using namespace client;

participant_creator_creator::participant_creator_creator(
    peer_creator &peer_creator_, tracks_adder &tracks_adder_)
    : peer_creator_(peer_creator_), tracks_adder_(tracks_adder_) {}

std::unique_ptr<participant_creator>
participant_creator_creator::create(const std::string &own_id) {
  return std::make_unique<participant_creator>(peer_creator_, tracks_adder_,
                                               own_id);
}
