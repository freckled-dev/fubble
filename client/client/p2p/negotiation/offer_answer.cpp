#include "offer_answer.hpp"

using namespace client::p2p::negotiation;

offer_answer::offer_answer(boost::executor &executor,
                           signalling::client::client &signalling_client,
                           rtc::connection &rtc_connection)
    : executor(executor), signalling_client(signalling_client),
      rtc_connection(rtc_connection) {
  rtc_connection.on_negotiation_needed.connect([this] { renegotiate(); });
  signalling_client.on_offer.connect([this](auto sdp) { on_offer(sdp); });
  signalling_client.on_answer.connect([this](auto sdp) { on_answer(sdp); });
  signalling_client.on_create_offer.connect([this] { on_create_offer(); });
}

void offer_answer::on_create_offer() {
  offering = true;
  renegotiate();
}

void offer_answer::renegotiate() {
  if (!offering)
    return;
  auto sdp = rtc_connection.create_offer();
  sdp.then(executor, [this](auto offer_future) {
    auto offer = offer_future.get();
    rtc_connection.set_local_description(offer);
    signalling::offer offer_casted{offer.sdp};
    signalling_client.send_offer(offer_casted);
  });
}

void offer_answer::on_answer(signalling::answer sdp) {
  rtc::session_description answer_casted{rtc::session_description::type::answer,
                                         sdp.sdp};
  rtc_connection.set_remote_description(answer_casted);
}

void offer_answer::on_offer(signalling::offer sdp) {
  rtc::session_description sdp_casted{rtc::session_description::type::offer,
                                      sdp.sdp};
  rtc_connection.set_remote_description(sdp_casted);
  rtc_connection.create_answer().then(executor, [this](auto answer_future) {
    auto answer = answer_future.get();
    rtc_connection.set_local_description(answer);
    signalling::answer answer_casted{answer.sdp};
    signalling_client.send_answer(answer_casted);
  });
}
