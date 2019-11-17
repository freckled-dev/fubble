#include "connection.hpp"

using namespace rtc::google;

connection::~connection() = default;

void connection::initialise(
    rtc::scoped_refptr<::webrtc::PeerConnectionInterface> native_) {
  BOOST_ASSERT(!native);
  BOOST_ASSERT(native_);
  native = native_;
}

boost::future<rtc::session_description> connection::create_offer() {
  boost::promise<session_description> promise;
  auto result = promise.get_future();
  return result;
}

boost::future<void>
connection::set_local_description(const session_description &) {
  boost::promise<void> promise;
  auto result = promise.get_future();
  return result;
}

boost::future<void>
connection::set_remote_description(const session_description &) {
  boost::promise<void> promise;
  auto result = promise.get_future();
  return result;
}

void connection::add_track(track_ptr) {}

void connection::OnSignalingChange(
    ::webrtc::PeerConnectionInterface::SignalingState new_state) {
  (void)new_state;
}

void connection::OnAddTrack(
    rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
    const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>
        &streams) {
  (void)receiver;
  (void)streams;
}

void connection::OnDataChannel(
    ::rtc::scoped_refptr<::webrtc::DataChannelInterface> data_channel) {
  (void)data_channel;
}

void connection::OnRenegotiationNeeded() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "OnRenegotiationNeeded";
  on_negotiation_needed();
}

void connection::OnIceGatheringChange(
    webrtc::PeerConnectionInterface::IceGatheringState new_state) {
  (void)new_state;
}

void connection::OnIceCandidate(
    const ::webrtc::IceCandidateInterface *candidate) {
  (void)candidate;
}
