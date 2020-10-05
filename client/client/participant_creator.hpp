#ifndef UUID_5ACD0027_5192_4804_B778_E80244B84D45
#define UUID_5ACD0027_5192_4804_B778_E80244B84D45

#include "client/desktop_sharing.hpp"
#include "client/participant.hpp"

namespace client {
class peer_creator;
class tracks_adder;
class own_media;
class factory;
class participant_creator {
public:
  // TODO maybe move `tracks_adder_` to `peer_creator` and into `peer`?
  participant_creator(factory &factory_, peer_creator &peer_creator_,
                      tracks_adder &tracks_adder_, const std::string &own_id,
                      own_media &own_media_,
                      const std::shared_ptr<desktop_sharing> desktop_sharing_);

  std::unique_ptr<participant> create(matrix::user &session_information);

protected:
  client::logger logger{"participant_creator"};
  factory &factory_;
  peer_creator &peer_creator_;
  tracks_adder &tracks_adder_;
  const std::string own_id;
  own_media &own_media_;
  const std::shared_ptr<desktop_sharing> desktop_sharing_;
};
} // namespace client

#endif
