#ifndef CLIENT_PEER_HPP
#define CLIENT_PEER_HPP

#include "p2p/negotiation/ice_candidates.hpp"
#include "p2p/negotiation/offer_answer.hpp"
#include "signalling/client/client.hpp"
#include <memory>

// TODO move to p2p
// TODO rtc stuff gets called from a different thread!
namespace client {
class peer {
public:
  peer(boost::executor &executor,
       std::unique_ptr<signalling::client::client> signalling_,
       std::unique_ptr<rtc::connection> rtc_);

  void connect(const std::string &key);
  boost::future<void> close();
  rtc::connection &rtc_connection();
  const rtc::connection &rtc_connection() const;

protected:
  std::unique_ptr<signalling::client::client> signalling_client;
  std::unique_ptr<rtc::connection> rtc_connection_;
  client::p2p::negotiation::ice_candidates ice_candidate_handler;
  client::p2p::negotiation::offer_answer offer_answer_handler;
};
} // namespace client

#endif
