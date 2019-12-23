#ifndef UUID_B27E182A_AF56_48E7_B9B3_428F3B393E2B
#define UUID_B27E182A_AF56_48E7_B9B3_428F3B393E2B

#include "logging/logger.hpp"

namespace rtc::google {
class factory;
}
namespace signalling::client {
class client_creator;
}
namespace client {
class peer;
class joiner {
public:
  joiner(signalling::client::client_creator &client_creator,
         rtc::google::factory &connection_creator);

  struct parameters {
    std::string name, room;
  };
  std::shared_ptr<peer> operator()(const parameters &parameters_);

protected:
  logging::logger logger;
  signalling::client::client_creator &client_creator;
  rtc::google::factory &connection_creator;
};
} // namespace client

#endif
