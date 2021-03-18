#ifndef UUID_DF21A153_FFF1_4F29_9F41_B783B7F0E0D3
#define UUID_DF21A153_FFF1_4F29_9F41_B783B7F0E0D3

#include <boost/thread/executor.hpp>
#include <fubble/client/peer.hpp>
#include <fubble/rtc/factory.hpp>

namespace signaling::client {
class factory;
}
namespace client {
class peer_creator {
public:
  peer_creator(boost::executor &executor,
               signaling::client::factory &client_creator,
               std::shared_ptr<rtc::factory> connection_creator,
               const peer::config &config_);

  std::unique_ptr<peer> create();

protected:
  boost::executor &executor;
  signaling::client::factory &client_creator;
  std::shared_ptr<rtc::factory> connection_creator;
  const peer::config config_;
};
} // namespace client

#endif
