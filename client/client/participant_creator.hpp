#ifndef UUID_5ACD0027_5192_4804_B778_E80244B84D45
#define UUID_5ACD0027_5192_4804_B778_E80244B84D45

#include "participant.hpp"
#include "session/participant.hpp"

namespace client {
class peer_creator;
class participant_creator {
public:
  participant_creator(peer_creator &peer_creator_, const std::string &own_id);

  std::unique_ptr<participant>
  create(const session::participant &session_information);

protected:
  peer_creator &peer_creator_;
  const std::string own_id;
};
} // namespace client

#endif
