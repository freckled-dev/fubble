#ifndef CLIENT_PEER_HPP
#define CLIENT_PEER_HPP

#include "fubble/client/logger.hpp"
#include "fubble/signaling/client/client.hpp"
#include "p2p/negotiation/ice_candidates.hpp"
#include "p2p/negotiation/offer_answer.hpp"
#include <memory>

// TODO move to p2p
namespace client {
class peer {
public:
  struct config {
    bool receive_audio{true};
    bool receive_video{true};
  };
  peer(boost::executor &executor,
       std::unique_ptr<signaling::client::client> signaling_,
       std::unique_ptr<rtc::connection> rtc_, const config &config_);
  ~peer();

  void connect(const std::string &token, const std::string &key);
  boost::future<void> close();
  rtc::connection &rtc_connection();
  const rtc::connection &rtc_connection() const;

protected:
  client::logger logger{"peer"};
  std::unique_ptr<signaling::client::client> signaling_client;
  std::unique_ptr<rtc::connection> rtc_connection_;
  client::p2p::negotiation::ice_candidates ice_candidate_handler;
  client::p2p::negotiation::offer_answer offer_answer_handler;
  const config config_;
};
} // namespace client

#endif
