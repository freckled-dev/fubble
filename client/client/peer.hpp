#ifndef CLIENT_PEER_HPP
#define CLIENT_PEER_HPP

#include "p2p/negotiation/ice_candidates.hpp"
#include "p2p/negotiation/offer_answer.hpp"
#include "rtc/connection.hpp"
#include "signalling/client/client.hpp"
#include <memory>

// TODO move to p2p
namespace client {
class peer {
public:
  peer(boost::executor &executor,
       std::unique_ptr<signalling::client::client> signalling_,
       std::unique_ptr<rtc::connection> rtc_);

protected:
  std::unique_ptr<signalling::client::client> signalling_client;
  std::unique_ptr<rtc::connection> rtc_connection;
  client::p2p::negotiation::ice_candidates ice_candidate_handler;
  client::p2p::negotiation::offer_answer offer_answer_handler;
};
} // namespace client

#endif

