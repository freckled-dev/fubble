#ifndef CLIENT_P2P_NEGOTIATION_OFFER_ANSWER_HPP
#define CLIENT_P2P_NEGOTIATION_OFFER_ANSWER_HPP

#include "rtc/connection.hpp"
#include "signalling/client/client.hpp"
#include <boost/thread/executors/executor.hpp>

namespace client::p2p::negotiation {
class offer_answer {
public:
  offer_answer(boost::executor &executor,
               signalling::client::client &signalling_client,
               rtc::connection &rtc_connection);

protected:
  void renegotiate();
  void on_create_offer();
  void on_answer(signalling::answer sdp);
  void on_offer(signalling::offer sdp);

  logging::logger logger;
  boost::executor &executor;
  signalling::client::client &signalling_client;
  rtc::connection &rtc_connection;
  bool offering{false};
};
} // namespace client::p2p::negotiation

#endif

