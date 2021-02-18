#include "peer.hpp"

using namespace client;

peer::peer(boost::executor &executor,
           std::unique_ptr<signaling::client::client> signaling_moved,
           std::unique_ptr<rtc::connection> rtc_moved, const config &config_)
    : signaling_client(std::move(signaling_moved)),
      rtc_connection_(std::move(rtc_moved)),
      ice_candidate_handler(*signaling_client, *rtc_connection_),
      offer_answer_handler(executor, *signaling_client, *rtc_connection_,
                           {config_.receive_audio, config_.receive_video}),
      config_{config_} {}

peer::~peer() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
}

void peer::connect(const std::string &token, const std::string &key) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  signaling_client->connect(token, key);
}

boost::future<void> peer::close() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  rtc_connection_->close();
  return signaling_client->close();
}

rtc::connection &peer::rtc_connection() { return *rtc_connection_; }

const rtc::connection &peer::rtc_connection() const { return *rtc_connection_; }
