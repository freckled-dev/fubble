#include "peer_creator.hpp"
#include "fubble/rtc/factory.hpp"
#include "fubble/signaling/client/client.hpp"
#include "peer.hpp"

using namespace client;

peer_creator::peer_creator(boost::executor &executor,
                           signaling::client::factory &client_creator,
                           std::shared_ptr<rtc::factory> connection_creator,
                           const peer::config &config_)
    : executor(executor), client_creator(client_creator),
      connection_creator(connection_creator), config_{config_} {}

std::unique_ptr<peer> peer_creator::create() {
  auto signaling_ = client_creator.create();
  auto connection_ = connection_creator->create_connection();
  auto result = std::make_unique<peer>(executor, std::move(signaling_),
                                       std::move(connection_), config_);
  return result;
}
