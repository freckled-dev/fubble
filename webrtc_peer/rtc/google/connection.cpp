#include "connection.hpp"
#include "data_channel.hpp"
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

using namespace rtc::google;

connection::~connection() = default;

void connection::initialise(
    rtc::scoped_refptr<::webrtc::PeerConnectionInterface> native_) {
  BOOST_ASSERT(!native);
  BOOST_ASSERT(native_);
  native = native_;
}

boost::future<rtc::session_description> connection::create_offer() {
  create_session_description_observer_ =
      new rtc::RefCountedObject<create_session_description_observer>();
  create_session_description_observer_->result.type_ =
      ::rtc::session_description::type::offer;
  auto result = create_session_description_observer_->promise.get_future();
  const webrtc::PeerConnectionInterface::RTCOfferAnswerOptions offer_options;
  native->CreateOffer(create_session_description_observer_, offer_options);
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

connection::data_channel_ptr connection::create_data_channel() {
  auto label = boost::uuids::random_generator()();
  auto label_string = boost::uuids::to_string(label);
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "create_data_channel, label:" << label_string;
  auto native_data_channel = native->CreateDataChannel(label_string, nullptr);
  auto result = std::make_shared<data_channel>(native_data_channel);
  data_channels.push_back(result);
  return result;
}

void connection::close() { native->Close(); }

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

void connection::create_session_description_observer::OnSuccess(
    webrtc::SessionDescriptionInterface *description_) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "connection::create_session_description_observer::OnSuccess";
  std::unique_ptr<webrtc::SessionDescriptionInterface> description{
      description_};
  [[maybe_unused]] bool success = description->ToString(&result.sdp);
  BOOST_ASSERT(success);
  promise.set_value(result);
}

void connection::create_session_description_observer::OnFailure(
    const std::string &error) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "connection::create_session_description_observer::OnFailure, error:"
      << error;
  promise.set_exception(std::runtime_error(error));
}
