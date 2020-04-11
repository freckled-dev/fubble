#ifndef UUID_5ACD0027_5192_4804_B778_E80244B84D45
#define UUID_5ACD0027_5192_4804_B778_E80244B84D45

#include "participant.hpp"
#include "session/participant.hpp"

namespace client {
class peer_creator;
class tracks_adder;
class own_media;
class participant_creator {
public:
  // TODO maybe move `tracks_adder_` to `peer_creator` and into `peer`?
  participant_creator(peer_creator &peer_creator_, tracks_adder &tracks_adder_,
                      const std::string &own_id, own_media &own_media_);

  std::unique_ptr<participant>
  create(session::participant &session_information);

protected:
  peer_creator &peer_creator_;
  tracks_adder &tracks_adder_;
  const std::string own_id;
  own_media &own_media_;
};
} // namespace client

#endif
