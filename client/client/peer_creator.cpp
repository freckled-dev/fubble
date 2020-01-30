#include "peer_creator.hpp"
#include "peer.hpp"
#include "rtc/google/factory.hpp"
#include "signalling/client/client_creator.hpp"

using namespace client;

peer_creator::peer_creator(boost::executor &executor,
                           signalling::client::client_creator &client_creator,
                           rtc::google::factory &connection_creator)
    : executor(executor), client_creator(client_creator),
      connection_creator(connection_creator) {}

std::unique_ptr<peer> peer_creator::create() {
  auto signalling_ = client_creator();
  auto connection_ = connection_creator.create_connection();
  auto result = std::make_unique<peer>(executor, std::move(signalling_),
                                       std::move(connection_));
  return result;
}
