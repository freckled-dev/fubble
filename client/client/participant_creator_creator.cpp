#include "participant_creator_creator.hpp"

using namespace client;

participant_creator_creator::participant_creator_creator(
    factory &factory_, peer_creator &peer_creator_, tracks_adder &tracks_adder_,
    own_media &own_media_,
    const std::shared_ptr<desktop_sharing> desktop_sharing_)
    : factory_(factory_), peer_creator_(peer_creator_),
      tracks_adder_(tracks_adder_),
      own_media_(own_media_), desktop_sharing_{desktop_sharing_} {}

std::unique_ptr<participant_creator>
participant_creator_creator::create(const std::string &own_id) {
  return std::make_unique<participant_creator>(factory_, peer_creator_,
                                               tracks_adder_, own_id,
                                               own_media_, desktop_sharing_);
}
