#include "client_creator.hpp"
#include "client.hpp"

using namespace signalling::client;

client_creator::client_creator(
    websocket::connector_creator &connector_creator,
    connection_creator &connection_creator_,
    const client::connect_information &connect_information_)
    : connector_creator(connector_creator),
      connection_creator_(connection_creator_),
      connect_information_(connect_information_) {}

std::unique_ptr<client> client_creator::create() {
  auto result = client::create(connector_creator, connection_creator_);
  result->set_connect_information(connect_information_);
  return result;
}
