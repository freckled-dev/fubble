#include "peer.hpp"

using namespace client;

peer::peer(std::unique_ptr<signalling::client::client> signalling_,
           std::unique_ptr<rtc::connection> rtc_)
    : signalling_client(std::move(signalling_)),
      rtc_connection(std::move(rtc_)) {}
