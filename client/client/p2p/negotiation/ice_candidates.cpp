#include "ice_candidates.hpp"

using namespace client::p2p::negotiation;

ice_candidates::ice_candidates(signalling::client::client &signalling_client,
                               rtc::connection &rtc_connection)
    : signalling_client(signalling_client), rtc_connection(rtc_connection) {
  rtc_connection.on_ice_candidate.connect(
      [this](const rtc::ice_candidate &candidate) { from_rtc(candidate); });
  signalling_client.on_ice_candidate.connect(
      [this](auto candidate) { from_signalling(candidate); });
}

void ice_candidates::from_rtc(const rtc::ice_candidate &candidate) {
  signalling::ice_candidate candidate_casted{candidate.mlineindex,
                                             candidate.mid, candidate.sdp};
  signalling_client.send_ice_candidate(candidate_casted);
}

void ice_candidates::from_signalling(
    const signalling::ice_candidate &candidate) {
  rtc::ice_candidate candidate_casted{candidate.mlineindex, candidate.mid,
                                      candidate.sdp};
  rtc_connection.add_ice_candidate(candidate_casted);
}
