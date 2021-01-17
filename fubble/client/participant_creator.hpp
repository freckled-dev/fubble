#ifndef UUID_5ACD0027_5192_4804_B778_E80244B84D45
#define UUID_5ACD0027_5192_4804_B778_E80244B84D45

#include "fubble/client/logger.hpp"
#include "fubble/client/participant.hpp"

namespace client {
class factory;
class own_media;
class peer_creator;
class peers;
class tracks_adder;
class participant_creator {
public:
  // TODO maybe move `tracks_adder_` to `peer_creator` and into `peer`?
  participant_creator(factory &factory_, std::shared_ptr<peers> peers_,
                      peer_creator &peer_creator_, tracks_adder &tracks_adder_,
                      const std::string &own_id, own_media &own_media_);
  ~participant_creator();

  std::unique_ptr<participant> create(matrix::user &session_information);

protected:
  client::logger logger{"participant_creator"};
  factory &factory_;
  std::shared_ptr<peers> peers_;
  peer_creator &peer_creator_;
  tracks_adder &tracks_adder_;
  const std::string own_id;
  own_media &own_media_;
};
} // namespace client

#endif
