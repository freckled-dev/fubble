#include "peer.hpp"

using namespace client;

peer::peer(boost::executor &executor,
           std::unique_ptr<signaling::client::client> signaling_moved,
           std::unique_ptr<rtc::connection> rtc_moved)
    : signaling_client(std::move(signaling_moved)),
      rtc_connection_(std::move(rtc_moved)),
      ice_candidate_handler(*signaling_client, *rtc_connection_),
      offer_answer_handler(executor, *signaling_client, *rtc_connection_) {}

void peer::connect(const std::string &token, const std::string &key) {
  signaling_client->connect(token, key);
}

boost::future<void> peer::close() {
  rtc_connection_->close();
  return signaling_client->close();
}

rtc::connection &peer::rtc_connection() { return *rtc_connection_; }

const rtc::connection &peer::rtc_connection() const { return *rtc_connection_; }
