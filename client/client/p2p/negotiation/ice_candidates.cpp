#include "ice_candidates.hpp"

using namespace client::p2p::negotiation;

ice_candidates::ice_candidates(signaling::client::client &signaling_client,
                               rtc::connection &rtc_connection)
    : signaling_client(signaling_client), rtc_connection(rtc_connection) {
  rtc_connection.on_ice_candidate.connect(
      [this](const rtc::ice_candidate &candidate) { from_rtc(candidate); });
  signaling_client.on_ice_candidate.connect(
      [this](auto candidate) { from_signaling(candidate); });
}

void ice_candidates::from_rtc(const rtc::ice_candidate &candidate) {
  signaling::ice_candidate candidate_casted{candidate.mlineindex,
                                             candidate.mid, candidate.sdp};
  signaling_client.send_ice_candidate(candidate_casted);
}

void ice_candidates::from_signaling(
    const signaling::ice_candidate &candidate) {
  rtc::ice_candidate candidate_casted{candidate.mlineindex, candidate.mid,
                                      candidate.sdp};
  rtc_connection.add_ice_candidate(candidate_casted);
}
