#include "connection.hpp"
#include "audio_track_sink.hpp"
#include "data_channel.hpp"
#include "uuid.hpp"
#include "video_track_sink.hpp"
#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <fmt/format.h>

#if defined(ABSL_HAVE_STD_OPTIONAL)
#if ABSL_HAVE_STD_OPTIONAL == 1
#error abseil header uses c++17 optional! but webrtc gets compiled with c++14!
#if __cplusplus >= 201703L
#error file not compiled with c++14. This will lead to problems with \
  abseil::optional
#endif
#endif
#endif

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

namespace {} // namespace

connection::connection()
    : logger{fmt::format("connection:{}", static_cast<void *>(this))} {}

connection::~connection() {
  BOOST_LOG_SEV(logger, logging::severity::debug)
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
connection::set_local_description(const rtc::session_description &description) {
  BOOST_LOG_SEV(logger, logging::severity::info) << "set_local_description";
  std::unique_ptr<::webrtc::SessionDescriptionInterface> casted;
  try {
    casted = cast_session_description(description);
  } catch (const std::runtime_error &error) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << __FUNCTION__ << ", an error occured while casting and setting, what:"
        << error.what();
    return boost::make_exceptional_future<void>(error);
  }
  auto observer = new rtc::RefCountedObject<set_session_description_observer>();
  // watchout. `SetLocalDescription` takes ownerhip
  auto result = observer->promise.get_future();
  native->SetLocalDescription(observer, casted.release());
  return result;
}

boost::future<void> connection::set_remote_description(
    const rtc::session_description &description) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << __FUNCTION__ << ", description:" << description.sdp.size();
  std::unique_ptr<::webrtc::SessionDescriptionInterface> casted;
  try {
    casted = cast_session_description(description);
  } catch (const std::runtime_error &error) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << __FUNCTION__ << ", an error occured while casting and setting, what:"
        << error.what();
    return boost::make_exceptional_future<void>(error);
  }
  auto observer = new rtc::RefCountedObject<set_remote_description_observer>();
  auto result = observer->promise.get_future();
  native->SetRemoteDescription(std::move(casted), observer);
  return result;
}

void connection::add_ice_candidate(const rtc::ice_candidate &candidate) {
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
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", track:" << track_.get();
  BOOST_ASSERT(track_);
  BOOST_ASSERT(find_sending_track(track_) == sending_tracks.cend());
  auto track_casted = std::dynamic_pointer_cast<track>(track_);
  BOOST_ASSERT(track_casted);
  auto native_track = track_casted->native_track();
  BOOST_ASSERT(native_track);
  webrtc::RTCErrorOr<rtc::scoped_refptr<webrtc::RtpSenderInterface>> result =
      native->AddTrack(native_track, {});
  if (!result.ok()) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "could not add track! state:" << native->signaling_state();
    BOOST_ASSERT(false);
    return;
  }
  sending_track add;
  add.track_ = track_;
  add.rtp = result.value();
  sending_tracks.push_back(std::move(add));
}

void connection::remove_track(rtc::track_ptr track_) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", track:" << track_.get();
  auto found = find_sending_track(track_);
  BOOST_ASSERT(found != sending_tracks.cend());
  if (found == sending_tracks.cend()) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << __FUNCTION__ << "could not remove connection";
    return;
  }
  auto track_casted = std::dynamic_pointer_cast<track>(track_);
  BOOST_ASSERT(track_casted);
  auto native_track = track_casted->native_track();
  BOOST_ASSERT(native_track);
  if (native->peer_connection_state() ==
      webrtc::PeerConnectionInterface::PeerConnectionState::kClosed) {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "not removing track from the delegating-connection, because "
           "connection is PeerConnectionState::kClosed. we are still removing "
           "it from the wrapping instance";
  } else {
    bool result = native->RemoveTrack(found->rtp);
    BOOST_ASSERT(result);
    if (!result) {
      BOOST_LOG_SEV(logger, logging::severity::warning)
          << "could not remove track";
    }
  }
  sending_tracks.erase(found);
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
    BOOST_LOG_SEV(logger, logging::severity::error) << "OnAddTrack !result";
    BOOST_ASSERT_MSG(false, "implement");
    return;
  }
  receiving_track add;
  add.rtp = receiver;
  add.track_ = result;
  receiving_tracks.push_back(std::move(add));
  on_track_added(result);
  if (is_video)
    on_video_track_added(result);
  else
    on_audio_track_added(result);
}

void connection::OnRemoveTrack(
    rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) {
  auto found = find_receiving_track(receiver);
  BOOST_ASSERT(found != receiving_tracks.cend());
  on_track_removed(found->track_);
  receiving_tracks.erase(found);
}

::rtc::track_ptr connection::check_handle_video_track(
    ::webrtc::MediaStreamTrackInterface &interface_) {
  if (interface_.kind() != ::webrtc::MediaStreamTrackInterface::kVideoKind)
    return nullptr;
  auto track_casted = dynamic_cast<webrtc::VideoTrackInterface *>(&interface_);
  BOOST_ASSERT(track_casted);
  return std::make_shared<video_track_sink>(track_casted);
}

::rtc::track_ptr connection::check_handle_audio_track(
    ::webrtc::MediaStreamTrackInterface &interface_) {
  if (interface_.kind() != ::webrtc::MediaStreamTrackInterface::kAudioKind)
    return nullptr;
  rtc::scoped_refptr<webrtc::AudioTrackInterface> track_casted =
      static_cast<webrtc::AudioTrackInterface *>(&interface_);
  return std::make_shared<audio_track_sink>(track_casted);
}

std::vector<connection::sending_track>::iterator
connection::find_sending_track(const track_ptr &track_) {
  return std::find_if(
      sending_tracks.begin(), sending_tracks.end(),
      [&](const auto &check) { return track_ == check.track_; });
}

std::vector<connection::receiving_track>::iterator
connection::find_receiving_track(
    const rtc::scoped_refptr<webrtc::RtpReceiverInterface> &search) {
  return std::find_if(receiving_tracks.begin(), receiving_tracks.end(),
                      [&](const auto &check) { return search == check.rtp; });
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
  bool success = description->ToString(&result.sdp);
  (void)success;
  BOOST_ASSERT(success);
  promise.set_value(result);
}

void connection::create_session_description_observer::OnFailure(
    webrtc::RTCError error) {
  BOOST_LOG_SEV(logger, logging::severity::warning)
      << "connection::create_session_description_observer::OnFailure, error:"
      << error.message();
  // TODO do a propper exception!
  promise.set_exception(std::runtime_error(error.message()));
}

void connection::set_session_description_observer::OnSuccess() {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "connection::set_session_description_observer::OnSuccess";
  promise.set_value();
}

void connection::set_session_description_observer::OnFailure(
    webrtc::RTCError error) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "connection::set_session_description_observer::OnFailure, error:"
      << error.message();
  // TODO refactor to boost::exception
  promise.set_exception(std::runtime_error(error.message()));
}

void connection::set_remote_description_observer::
    OnSetRemoteDescriptionComplete(webrtc::RTCError error) {
  BOOST_LOG_SEV(logger, logging::severity::info) << __FUNCTION__;
  if (error.ok())
    return promise.set_value();
  BOOST_LOG_SEV(logger, logging::severity::error)
      << __FUNCTION__ << ", error:" << error.message();
  // TODO refactor to boost::exception
  promise.set_exception(std::runtime_error(error.message()));
}

std::unique_ptr<::webrtc::SessionDescriptionInterface>
connection::cast_session_description(
    const rtc::session_description &description) {
  webrtc::SdpParseError error;
  auto type_casted = [&] {
    if (description.type_ == rtc::session_description::type::answer)
      return webrtc::SdpType::kAnswer;
    BOOST_ASSERT(description.type_ == rtc::session_description::type::offer);
    return webrtc::SdpType::kOffer;
  }();
  auto casted =
      webrtc::CreateSessionDescription(type_casted, description.sdp, &error);
  if (casted)
    return casted;
  auto error_description =
      fmt::format("sdp parsing error, description:'{}', line:{}",
                  error.description, error.line);
  BOOST_LOG_SEV(logger, logging::severity::warning)
      << __FUNCTION__ << ", failed to cast description, error_description:"
      << error_description;
  throw std::runtime_error(error_description);
}
