#ifndef CLIENT_PEER_HPP
#define CLIENT_PEER_HPP

#include "rtc/connection.hpp"
#include "signalling/client/client.hpp"
#include <memory>

// TODO move to p2p
namespace client {
class peer {
public:
  peer(std::unique_ptr<signalling::client::client> signalling_,
       std::unique_ptr<rtc::connection> rtc_);

protected:
  std::unique_ptr<signalling::client::client> signalling_client;
  std::unique_ptr<rtc::connection> rtc_connection;
};
} // namespace client

#endif

