#ifndef CLIENT_P2P_NEGOTIATION_ICE_CANDIDATES_HPP
#define CLIENT_P2P_NEGOTIATION_ICE_CANDIDATES_HPP

#include "rtc/connection.hpp"
#include "signaling/client/client.hpp"

namespace client::p2p::negotiation {
class ice_candidates {
public:
  ice_candidates(signaling::client::client &signaling_client,
                 rtc::connection &rtc_connection);

protected:
  void from_rtc(const rtc::ice_candidate &candidate);
  void from_signaling(const signaling::ice_candidate &candidate);

  signaling::client::client &signaling_client;
  rtc::connection &rtc_connection;
};
} // namespace client::p2p::negotiation

#endif
