#include "joiner.hpp"
#include "peer.hpp"
#include "peer_creator.hpp"
#include "peers.hpp"
#include "rtc/connection.hpp"
#include "signalling/client/client.hpp"

using namespace client;

joiner::joiner(peers &peers_, peer_creator &peer_creator_)
    : peers_(peers_), peer_creator_(peer_creator_) {}

std::shared_ptr<peer> joiner::operator()(const parameters &parameters_) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "join(), name:" << parameters_.name << ", room:" << parameters_.room;
  std::shared_ptr<peer> result = peer_creator_();
  peers_.add(result);
  return result;
}
