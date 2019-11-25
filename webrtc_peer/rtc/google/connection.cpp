#include "connection.hpp"
#include "data_channel.hpp"
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <fmt/format.h>

using namespace rtc::google;

namespace {
std::unique_ptr<::webrtc::SessionDescriptionInterface>
cast_session_description(const rtc::session_description &description) {
  webrtc::SdpParseError error;
  auto type_casted = [&] {
    if (description.type_ == rtc::session_description::type::answer)
      return webrtc::SdpType::kAnswer;
    return webrtc::SdpType::kOffer;
  }();
  auto casted =
      webrtc::CreateSessionDescription(type_casted, description.sdp, &error);
  if (casted)
    return casted;
  auto error_description =
      fmt::format("sdp parsing error, description:'{}', line:{}",
                  error.description, error.line);
  throw std::runtime_error(error_description);
}
} // namespace

connection::~connection() = default;

void connection::initialise(
    rtc::scoped_refptr<::webrtc::PeerConnectionInterface> native_) {
  BOOST_ASSERT(!native);
  BOOST_ASSERT(native_);
  native = native_;
}

boost::future<rtc::session_description> connection::create_offer() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "create_offer";
  auto observer =
      new rtc::RefCountedObject<create_session_description_observer>();
  observer->result.type_ = ::rtc::session_description::type::offer;
  const webrtc::PeerConnectionInterface::RTCOfferAnswerOptions options;
  native->CreateOffer(observer, options);
  return observer->promise.get_future();
}

boost::future<rtc::session_description> connection::create_answer() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "create_answer";
  auto observer =
      new rtc::RefCountedObject<create_session_description_observer>();
  observer->result.type_ = ::rtc::session_description::type::answer;
  const webrtc::PeerConnectionInterface::RTCOfferAnswerOptions options;
  native->CreateAnswer(observer, options);
  return observer->promise.get_future();
}

boost::future<void>
connection::set_local_description(const session_description &description) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "set_local_description";
  try {
    auto casted = cast_session_description(description);
    auto observer =
        new rtc::RefCountedObject<set_session_description_observer>();
    // watchout. `SetLocalDescription` takes ownerhip
    native->SetLocalDescription(observer, casted.release());
    return observer->promise.get_future();
  } catch (...) {
    return boost::make_exceptional_future<void>();
  }
}

boost::future<void>
connection::set_remote_description(const session_description &description) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "set_remote_description";
  try {
    auto casted = cast_session_description(description);
    auto observer =
        new rtc::RefCountedObject<set_session_description_observer>();
    // watchout. `SetRemoteDescription` takes ownerhip
    native->SetRemoteDescription(observer, casted.release());
    return observer->promise.get_future();
  } catch (...) {
    return boost::make_exceptional_future<void>();
  }
}

void connection::add_ice_candidate(const ice_candidate &candidate) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "on_ice_candidate";
  webrtc::SdpParseError error;
  std::unique_ptr<webrtc::IceCandidateInterface> parsed{
      webrtc::CreateIceCandidate(candidate.mid, candidate.mlineindex,
                                 candidate.sdp, &error)};
  if (!parsed) {
    auto error_description = fmt::format(
        "could not parse ice_candidate, description:'{}', line:'{}'",
        error.description, error.line);
    throw std::runtime_error(error_description);
  }
  native->AddIceCandidate(parsed.get());
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
    ::rtc::scoped_refptr<::webrtc::DataChannelInterface> data_channel_) {
  auto result = std::make_shared<data_channel>(data_channel_);
  data_channels.push_back(result);
  on_data_channel(result);
}

void connection::OnRenegotiationNeeded() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "OnRenegotiationNeeded";
  on_negotiation_needed();
}

void connection::OnIceGatheringChange(
    webrtc::PeerConnectionInterface::IceGatheringState new_state) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "OnIceGatheringChange";
  (void)new_state;
}

void connection::OnIceCandidate(
    const ::webrtc::IceCandidateInterface *candidate) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "OnIceCandidate";
  ice_candidate result;
  result.mlineindex = candidate->sdp_mline_index();
  result.mid = candidate->sdp_mid();
  candidate->ToString(&result.sdp);
  on_ice_candidate(result);
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

void connection::set_session_description_observer::OnSuccess() {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "connection::set_session_description_observer::OnSuccess";
  promise.set_value();
}

void connection::set_session_description_observer::OnFailure(
    const std::string &error) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "connection::set_session_description_observer::OnFailure, error:"
      << error;
  promise.set_exception(std::runtime_error(error));
}
