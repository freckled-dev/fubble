#ifndef UUID_B27E182A_AF56_48E7_B9B3_428F3B393E2B
#define UUID_B27E182A_AF56_48E7_B9B3_428F3B393E2B

#include "logging/logger.hpp"

namespace client {
class peer;
class peers;
class peer_creator;
class joiner {
public:
  joiner(peers &peers_, peer_creator &peer_creator_);

  struct parameters {
    std::string name, room;
  };
  std::shared_ptr<peer> operator()(const parameters &parameters_);

protected:
  logging::logger logger;
  peers &peers_;
  peer_creator &peer_creator_;
};
} // namespace client

#endif
