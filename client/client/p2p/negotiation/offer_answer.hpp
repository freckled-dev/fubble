#ifndef CLIENT_P2P_NEGOTIATION_OFFER_ANSWER_HPP
#define CLIENT_P2P_NEGOTIATION_OFFER_ANSWER_HPP

#include "client/logger.hpp"
#include "rtc/connection.hpp"
#include "signalling/client/client.hpp"
#include <boost/thread/executors/executor.hpp>

namespace client::p2p::negotiation {
class offer_answer {
public:
  // TODO remove executor. A executor gets used here because google::webrtc
  // sometimes switches threads. Move the executor to google::webrtc
  offer_answer(boost::executor &executor,
               signalling::client::client &signalling_client,
               rtc::connection &rtc_connection);

protected:
  void on_negotiation_needed();
  void on_connected();
  void on_closed();
  void on_create_offer();
  void on_answer(signalling::answer sdp);
  void on_offer(signalling::offer sdp);
  void on_offer_set(boost::future<void> &set_result);

  client::logger logger{"offer_answer"};
  boost::executor &executor;
  signalling::client::client &signalling_client;
  rtc::connection &rtc_connection;
  bool connected{};
  bool wants_to_negotiate{};
};
} // namespace client::p2p::negotiation

#endif
