#include "peer.hpp"

using namespace client;

peer::peer(boost::executor &executor,
           std::unique_ptr<signalling::client::client> signalling_,
           std::unique_ptr<rtc::connection> rtc_)
    : signalling_client(std::move(signalling_)),
      rtc_connection_(std::move(rtc_)),
      ice_candidate_handler(*signalling_client, *rtc_connection_),
      offer_answer_handler(executor, *signalling_client, *rtc_connection_) {}

void peer::connect(const std::string &key) { signalling_client->connect(key); }
rtc::connection &peer::rtc_connection() { return *rtc_connection_; }
const rtc::connection &peer::rtc_connection() const { return *rtc_connection_; }
