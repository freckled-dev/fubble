#include "joiner.hpp"
#include "peer.hpp"
#include "rtc/connection.hpp"
#include "rtc/google/factory.hpp"
#include "signalling/client/client.hpp"
#include "signalling/client/client_creator.hpp"

using namespace client;

joiner::joiner(signalling::client::client_creator &client_creator,
               rtc::google::factory &connection_creator)
    : client_creator(client_creator), connection_creator(connection_creator) {}

std::shared_ptr<peer> joiner::operator()(const parameters &parameters_) {
  (void)parameters_;
  auto signalling_ = client_creator();
  auto connection_ = connection_creator.create_connection();
  auto result =
      std::make_shared<peer>(std::move(signalling_), std::move(connection_));
  return result;
}

