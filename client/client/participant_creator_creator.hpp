#ifndef UUID_FEE2707B_5A9F_4D54_B716_D020BB228278
#define UUID_FEE2707B_5A9F_4D54_B716_D020BB228278

#include "participant_creator.hpp"

namespace client {
class own_media;
// TODO this class is very small. make header only
class participant_creator_creator {
public:
  participant_creator_creator(peer_creator &peer_creator_,
                              tracks_adder &tracks_adder_,
                              own_media &own_media_);

  std::unique_ptr<participant_creator> create(const std::string &own_id);

protected:
  peer_creator &peer_creator_;
  tracks_adder &tracks_adder_;
  own_media &own_media_;
};
} // namespace client

#endif
