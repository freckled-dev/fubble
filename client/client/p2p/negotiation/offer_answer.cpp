#include "offer_answer.hpp"

using namespace client::p2p::negotiation;

offer_answer::offer_answer(boost::executor &executor,
                           signalling::client::client &signalling_client,
                           rtc::connection &rtc_connection)
    : executor(executor), signalling_client(signalling_client),
      rtc_connection(rtc_connection) {
  rtc_connection.on_negotiation_needed.connect(
      [this] { on_negotiation_needed(); });
  signalling_client.on_offer.connect([this](auto sdp) { on_offer(sdp); });
  signalling_client.on_answer.connect([this](auto sdp) { on_answer(sdp); });
  signalling_client.on_create_offer.connect([this] { on_create_offer(); });
  signalling_client.on_registered.connect([this] { on_connected(); });
  signalling_client.on_closed.connect([this] { on_closed(); });
}

void offer_answer::on_create_offer() {
  auto sdp = rtc_connection.create_offer();
  sdp.then(executor,
           [this](auto offer_future) {
             auto offer = offer_future.get();
             return rtc_connection.set_local_description(offer).then(
                 executor, [this, offer](auto local_description_result) {
                   local_description_result.get();
                   signalling::offer offer_casted{offer.sdp};
                   return signalling_client.send_offer(offer_casted);
                 });
           })
      .unwrap();
}

void offer_answer::on_negotiation_needed() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "on_negotiation_needed";
  if (connected) {
    signalling_client.send_want_to_negotiate();
    return;
  }
  wants_to_negotiate = true;
}

void offer_answer::on_connected() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "on_connected";
  connected = true;
  if (!wants_to_negotiate)
    return;
  signalling_client.send_want_to_negotiate();
  wants_to_negotiate = false;
}

void offer_answer::on_closed() { connected = false; }

void offer_answer::on_answer(signalling::answer sdp) {
  rtc::session_description answer_casted{rtc::session_description::type::answer,
                                         sdp.sdp};
  rtc_connection.set_remote_description(answer_casted);
}

void offer_answer::on_offer(signalling::offer sdp) {
  rtc::session_description sdp_casted{rtc::session_description::type::offer,
                                      sdp.sdp};
  rtc_connection.set_remote_description(sdp_casted)
      .then(executor, [this](auto result) { on_offer_set(result); });
}

void offer_answer::on_offer_set(boost::future<void> &set_result) {
  try {
    set_result.get();
  } catch (...) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << __FUNCTION__ << ", could not set offer as remote_description";
    return;
  }
  rtc_connection.create_answer().then(executor, [this](auto answer_future) {
    try {
      auto answer = answer_future.get();
      rtc_connection.set_local_description(answer).then(
          executor, [answer, this](auto local_description_result) {
            local_description_result.get();
            signalling::answer answer_casted{answer.sdp};
            signalling_client.send_answer(answer_casted);
          });
    } catch (...) {
      BOOST_LOG_SEV(this->logger, logging::severity::error)
          << __FUNCTION__ << ", could not create answer";
      return;
    }
  });
}
