#ifndef UUID_FEE2707B_5A9F_4D54_B716_D020BB228278
#define UUID_FEE2707B_5A9F_4D54_B716_D020BB228278

#include "participant_creator.hpp"

namespace client {
class participant_creator_creator {
public:
  participant_creator_creator(peer_creator &peer_creator_);

  std::unique_ptr<participant_creator> create(const std::string &own_id);

protected:
  peer_creator &peer_creator_;
};
} // namespace client

#endif
