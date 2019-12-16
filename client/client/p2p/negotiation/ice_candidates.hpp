#ifndef CLIENT_P2P_NEGOTIATION_ICE_CANDIDATES_HPP
#define CLIENT_P2P_NEGOTIATION_ICE_CANDIDATES_HPP

#include "rtc/connection.hpp"
#include "signalling/client/client.hpp"

namespace client::p2p::negotiation {
class ice_candidates {
public:
  ice_candidates(signalling::client::client &signalling_client,
                 rtc::connection &rtc_connection);

protected:
  void from_rtc(const rtc::ice_candidate &candidate);
  void from_signalling(const signalling::ice_candidate &candidate);

  signalling::client::client &signalling_client;
  rtc::connection &rtc_connection;
};
} // namespace client::p2p::negotiation

#endif
