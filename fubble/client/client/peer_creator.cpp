#include "peer_creator.hpp"
#include "fubble/rtc/google/connection.hpp"
#include "fubble/signaling/client/client.hpp"
#include "peer.hpp"
#include "rtc/google/factory.hpp"

using namespace client;

peer_creator::peer_creator(boost::executor &executor,
                           signaling::client::factory &client_creator,
                           rtc::google::factory &connection_creator)
    : executor(executor), client_creator(client_creator),
      connection_creator(connection_creator) {}

std::unique_ptr<peer> peer_creator::create() {
  auto signaling_ = client_creator.create();
  auto connection_ = connection_creator.create_connection();
  auto result = std::make_unique<peer>(executor, std::move(signaling_),
                                       std::move(connection_));
  return result;
}
