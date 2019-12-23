#include "client_creator.hpp"
#include "client.hpp"

using namespace signalling::client;

client_creator::client_creator(boost::executor &executor,
                               websocket::connector &connector,
                               connection_creator &connection_creator_)
    : executor(executor), connector(connector),
      connection_creator_(connection_creator_) {}

std::unique_ptr<client> client_creator::operator()() {
  return std::make_unique<client>(executor, connector, connection_creator_);
}

