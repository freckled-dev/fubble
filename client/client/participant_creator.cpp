#include "participant_creator.hpp"
#include "bot_participant.hpp"
#include "matrix/user.hpp"
#include "own_participant.hpp"
#include "peer_creator.hpp"
#include "remote_participant.hpp"
#include "tracks_adder.hpp"

using namespace client;

participant_creator::participant_creator(factory &factory_,
                                         peer_creator &peer_creator_,
                                         tracks_adder &tracks_adder_,
                                         const std::string &own_id,
                                         own_media &own_media_)
    : factory_(factory_), peer_creator_(peer_creator_),
      tracks_adder_(tracks_adder_), own_id(own_id), own_media_(own_media_) {}

std::unique_ptr<participant>
participant_creator::create(matrix::user &session_information) {
  if (session_information.get_id() == own_id)
    return std::make_unique<own_participant>(session_information, own_media_);
  if (session_information.get_display_name() == "Fubble Bot")
    return std::make_unique<bot_participant>(session_information);
  auto peer = peer_creator_.create();
  auto peer_pointer = peer.get();
  auto result = std::make_unique<remote_participant>(factory_, std::move(peer),
                                                     session_information);
  auto other_id = session_information.get_id();
  const std::string peer_id = [&] {
    if (own_id < other_id)
      return own_id + '_' + other_id;
    return other_id + '_' + own_id;
  }();
  tracks_adder_.add_to_connection(peer_pointer->rtc_connection());
  peer_pointer->connect(peer_id);
  return result;
}
