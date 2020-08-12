#ifndef UUID_DF21A153_FFF1_4F29_9F41_B783B7F0E0D3
#define UUID_DF21A153_FFF1_4F29_9F41_B783B7F0E0D3

#include "peer.hpp"
#include <boost/thread/executor.hpp>

namespace rtc::google {
class factory;
}
namespace signalling::client {
class factory;
}
namespace client {
class peer_creator {
public:
  peer_creator(boost::executor &executor,
               signalling::client::factory &client_creator,
               rtc::google::factory &connection_creator);

  std::unique_ptr<peer> create();

protected:
  boost::executor &executor;
  signalling::client::factory &client_creator;
  rtc::google::factory &connection_creator;
};
} // namespace client

#endif
