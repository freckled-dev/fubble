#include "joiner.hpp"
#include "add_video_to_connection.hpp"
#include "peer.hpp"
#include "peer_creator.hpp"
#include "peers.hpp"
#include "rtc/connection.hpp"
#include "signalling/client/client.hpp"

using namespace client;

joiner::joiner(peers &peers_, add_video_to_connection &track_adder,
               peer_creator &peer_creator_)
    : peers_(peers_), track_adder(track_adder), peer_creator_(peer_creator_) {}

std::shared_ptr<peer> joiner::operator()(const parameters &parameters_) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "join(), name:" << parameters_.name << ", room:" << parameters_.room;
  std::shared_ptr<peer> result = peer_creator_();
  track_adder.add_to_connection(result->rtc_connection());
  peers_.add(result);
  result->connect(parameters_.room);
  return result;
}
