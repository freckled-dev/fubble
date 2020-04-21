#include "connection.hpp"
#include "audio_track_sink.hpp"
#include "data_channel.hpp"
#include "track.hpp"
#include "uuid.hpp"
#include "video_track_sink.hpp"
#include <fmt/format.h>

namespace std {
std::ostream &
operator<<(std::ostream &out,
           const webrtc::PeerConnectionInterface::SignalingState print) {
  const auto render = [&](const std::string &name) -> std::ostream & {
    return out << name << "(" << static_cast<int>(print) << ")";
  };
  switch (print) {
  case webrtc::PeerConnectionInterface::kStable:
    return render("kStable");
  case webrtc::PeerConnectionInterface::kHaveLocalOffer:
    return render("kHaveLocalOffer");
  case webrtc::PeerConnectionInterface::kHaveLocalPrAnswer:
    return render("kHaveLocalPrAnswer");
  case webrtc::PeerConnectionInterface::kHaveRemoteOffer:
    return render("kHaveRemoteOffer");
  case webrtc::PeerConnectionInterface::kHaveRemotePrAnswer:
    return render("kHaveRemotePrAnswer");
  case webrtc::PeerConnectionInterface::kClosed:
    return render("kClosed");
  }
  BOOST_ASSERT_MSG(false, "must not reach");
  return out << "<undefined>";
}
std::ostream &
operator<<(std::ostream &out,
           const webrtc::PeerConnectionInterface::PeerConnectionState print) {
  const auto render = [&](const std::string &name) -> std::ostream & {
    return out << name << "(" << static_cast<int>(print) << ")";
  };
  switch (print) {
  case webrtc::PeerConnectionInterface::PeerConnectionState::kClosed:
    return render("kClosed");
  case webrtc::PeerConnectionInterface::PeerConnectionState::kConnected:
    return render("kConnected");
  case webrtc::PeerConnectionInterface::PeerConnectionState::kConnecting:
    return render("kConnecting");
  case webrtc::PeerConnectionInterface::PeerConnectionState::kDisconnected:
    return render("kDisconnected");
  case webrtc::PeerConnectionInterface::PeerConnectionState::kFailed:
    return render("kFailed");
  case webrtc::PeerConnectionInterface::PeerConnectionState::kNew:
    return render("kNew");
  }
  BOOST_ASSERT(false && "must not reach");
  return out << "<undefined>";
}
std::ostream &
operator<<(std::ostream &out,
           const webrtc::PeerConnectionInterface::IceConnectionState print) {
  const auto render = [&](const std::string &name) -> std::ostream & {
    return out << name << "(" << static_cast<int>(print) << ")";
  };
  switch (print) {
  case webrtc::PeerConnectionInterface::kIceConnectionNew:
    return render("kIceConnectionNew");
  case webrtc::PeerConnectionInterface::kIceConnectionChecking:
    return render("kIceConnectionChecking");
  case webrtc::PeerConnectionInterface::kIceConnectionConnected:
    return render("kIceConnectionConnected");
  case webrtc::PeerConnectionInterface::kIceConnectionCompleted:
    return render("kIceConnectionCompleted");
  case webrtc::PeerConnectionInterface::kIceConnectionFailed:
    return render("kIceConnectionFailed");
  case webrtc::PeerConnectionInterface::kIceConnectionDisconnected:
    return render("kIceConnectionDisconnected");
  case webrtc::PeerConnectionInterface::kIceConnectionClosed:
    return render("kIceConnectionClosed");
  case webrtc::PeerConnectionInterface::kIceConnectionMax:
    return render("kIceConnectionMax");
  }
  return out << "<undefined>";
}
} // namespace std

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

connection::~connection() {
  BOOST_LOG_SEV(logger, logging::severity::trace)
      << "google::webrtc::connection::~connection(), this:" << this;
}

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
  auto result = observer->promise.get_future();
  native->CreateOffer(observer, options);
  return result;
}

boost::future<rtc::session_description> connection::create_answer() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "create_answer";
  auto observer =
      new rtc::RefCountedObject<create_session_description_observer>();
  observer->result.type_ = ::rtc::session_description::type::answer;
  const webrtc::PeerConnectionInterface::RTCOfferAnswerOptions options;
  auto result = observer->promise.get_future();
  native->CreateAnswer(observer, options);
  return result;
}

boost::future<void>
connection::set_local_description(const session_description &description) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "set_local_description";
  try {
    auto casted = cast_session_description(description);
    auto observer =
        new rtc::RefCountedObject<set_session_description_observer>();
    // watchout. `SetLocalDescription` takes ownerhip
    auto result = observer->promise.get_future();
    native->SetLocalDescription(observer, casted.release());
    return result;
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
    auto result = observer->promise.get_future();
    native->SetRemoteDescription(observer, casted.release());
    return result;
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

void connection::add_track(rtc::track_ptr track_) {
  auto track_casted = std::dynamic_pointer_cast<track>(track_);
  BOOST_ASSERT(track_casted);
  auto native_track = track_casted->native_track();
  BOOST_ASSERT(native_track);
  [[maybe_unused]] auto result = native->AddTrack(native_track, {});
  BOOST_ASSERT(result.ok());
  tracks.push_back(track_);
}

rtc::data_channel_ptr connection::create_data_channel() {
  auto label = uuid::generate();
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "create_data_channel, label:" << label;
  auto native_data_channel = native->CreateDataChannel(label, nullptr);
  auto result = std::make_shared<data_channel>(native_data_channel);
  data_channels.push_back(result);
  return result;
}

void connection::close() {
  // closes and destroys the data channels
  native->Close();
}

void connection::OnConnectionChange(
    webrtc::PeerConnectionInterface::PeerConnectionState new_state) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "OnConnectionChange, new_state:" << new_state;
}
void connection::OnSignalingChange(
    ::webrtc::PeerConnectionInterface::SignalingState new_state) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "OnSignalingChange, new_state:" << new_state;
}

void connection::OnStandardizedIceConnectionChange(
    webrtc::PeerConnectionInterface::IceConnectionState new_state) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "OnStandardizedIceConnectionChange, new_state:" << new_state;
}

void connection::OnAddTrack(
    rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
    const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>
        & /*streams*/) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "OnAddTrack";
  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track =
      receiver->track();
  BOOST_ASSERT(track);
  bool is_video{true};
  auto result = check_handle_video_track(*track);
  if (!result) {
    result = check_handle_audio_track(*track);
    is_video = false;
  }
  if (!result) {
    BOOST_ASSERT_MSG(false, "implement");
    return;
  }
  tracks.push_back(result);
  on_track(result);
  if (is_video)
    on_video_track(result);
  else
    on_audio_track(result);
}

rtc::track_ptr connection::check_handle_video_track(
    webrtc::MediaStreamTrackInterface &interface) {
  auto track_casted = dynamic_cast<webrtc::VideoTrackInterface *>(&interface);
  if (track_casted == nullptr)
    return nullptr;
  return std::make_shared<video_track_sink>(track_casted);
}

rtc::track_ptr connection::check_handle_audio_track(
    webrtc::MediaStreamTrackInterface &interface) {
  rtc::scoped_refptr<webrtc::AudioTrackInterface> track_casted =
      dynamic_cast<webrtc::AudioTrackInterface *>(&interface);
  if (track_casted == nullptr)
    return nullptr;
  return std::make_shared<audio_track_sink>(track_casted);
}

void connection::OnDataChannel(
    ::rtc::scoped_refptr<::webrtc::DataChannelInterface> data_channel_) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "OnDataChannel, this:" << this;
  data_channel_ptr result = std::make_shared<data_channel>(data_channel_);
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
