#include "logging/logger.hpp"
#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/create_peerconnection_factory.h>
#include <api/peer_connection_interface.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>
#include <boost/signals2/signal.hpp>
#include <rtc_base/ssl_adapter.h>
// #include <modules/audio_device/include/audio_device.h>
// #include <modules/audio_processing/include/audio_processing.h>
// #include <rtc_base/flags.h>
// #include <rtc_base/ssladapter.h>
// #include <api/create_peerconnection_factory.h>
// #include <api/peer_connection_interface.h>

namespace {
#if 0
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
  return out << "<undefined>";
}

std::ostream &
operator<<(std::ostream &out,
           const webrtc::PeerConnectionInterface::IceConnectionState print) {
  const auto render = [&](const std::string &name) -> std::ostream & {
    return out << name << "(" << static_cast<int>(print) << ")";
  };
  switch (print) {
  case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionNew:
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
#endif

class signaling {
public:
  void send_ice_candidate(const std::string &candidate,
                          const std::string &sdp_mid,
                          const int sdp_mline_index) {
    (void)candidate;
    (void)sdp_mid;
    (void)sdp_mline_index;
  }
  void send_offer(const std::string &sdp) { (void)sdp; }
  void send_answer(const std::string &sdp) { (void)sdp; }
  boost::signals2::signal<void(std::string)> on_offer;
  boost::signals2::signal<void(std::string)> on_answer;
};

class data_channel_observer : public webrtc::DataChannelObserver {
public:
  rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel;
  logging::logger logger;

  void OnStateChange() override {
    BOOST_LOG_SEV(logger, logging::severity::info) << "OnStateChange";
  }
  void OnMessage(const webrtc::DataBuffer &buffer) override {
    const std::string message(buffer.data.data<char>(), buffer.data.size());
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "OnMessage, buffer.binary:" << buffer.binary
        << ", message.size():" << message.size();
    BOOST_LOG_SEV(logger, logging::severity::info) << message;
    const std::string answer = "got the message:" + message;
    const webrtc::DataBuffer send_buffer{answer};
    const bool result = data_channel->Send(send_buffer);
    if (!result)
      throw std::runtime_error("OnMessage !result");
  }

  void OnBufferedAmountChange(uint64_t previous_amount) override {
    const auto current_amount = data_channel->buffered_amount();
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "OnBufferedAmountChange, previous_amount:" << previous_amount
        << ", current_amount:" << current_amount;
  }
};

class peer_connection_observer : public ::webrtc::PeerConnectionObserver {
public:
  signaling &signaling_;
  data_channel_observer &data_channel_observer_;
  logging::logger logger;

  peer_connection_observer(signaling &signaling_,
                           data_channel_observer &data_channel_observer_)
      : signaling_(signaling_), data_channel_observer_(data_channel_observer_) {
  }

  void OnSignalingChange(
      webrtc::PeerConnectionInterface::SignalingState new_state) override {
    // https://w3c.github.io/webrtc-pc/#rtcpeerconnectionstate-enum
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "OnSignalingChange, new_state:" << new_state;
  }
  void OnDataChannel(
      rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override {
    BOOST_LOG_SEV(logger, logging::severity::info) << "OnDataChannel";
    data_channel_observer_.data_channel = data_channel;
    data_channel->RegisterObserver(&data_channel_observer_);
  }
  void OnRenegotiationNeeded() override {
    BOOST_LOG_SEV(logger, logging::severity::info) << "OnRenegotiationNeeded";
  }
  void OnIceConnectionChange(
      webrtc::PeerConnectionInterface::IceConnectionState new_state) override {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "OnIceConnectionChange, new_state:" << new_state;
  }
  void OnIceGatheringChange(
      webrtc::PeerConnectionInterface::IceGatheringState new_state) override {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "OnIceGatheringChange, new_state:" << new_state;
  }
  void OnIceCandidate(const webrtc::IceCandidateInterface *candidate) override {
    std::string candidate_send;
    candidate->ToString(&candidate_send);
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "OnIceCandidate:" << candidate_send;
    const std::string sdpMid = candidate->sdp_mid();
    const int sdpMLineIndex = candidate->sdp_mline_index();
    signaling_.send_ice_candidate(candidate_send, sdpMid, sdpMLineIndex);
  }

  // PeerConnectionObserver interface
public:
  void OnAddStream(
      rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override {
    BOOST_LOG_SEV(logger, logging::severity::info) << "OnAddStream";
    ::webrtc::PeerConnectionObserver::OnAddStream(stream);
  }
  void
  OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
             const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>
                 &streams) override {
    BOOST_LOG_SEV(logger, logging::severity::info) << "OnAddTrack";
    ::webrtc::PeerConnectionObserver::OnAddTrack(receiver, streams);
  }
  void OnTrack(rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver)
      override {
    BOOST_LOG_SEV(logger, logging::severity::info) << "OnTrack";
    ::webrtc::PeerConnectionObserver::OnTrack(transceiver);
  }
};

class set_session_description_observer
    : public webrtc::SetSessionDescriptionObserver {
public:
  set_session_description_observer(const std::string &description)
      : description(description) {}
  ~set_session_description_observer() override = default;

  void OnSuccess() override {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "set_session_description_observer::OnSuccess, description:"
        << description;
  }
  void OnFailure(const std::string &error) override {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "set_session_description_observer::OnFailure, description:"
        << description;
    throw std::runtime_error(error);
  }

  std::string description;
  logging::logger logger;
};

std::string make_sdp_parse_error_description(webrtc::SdpParseError error,
                                             std::string description) {
  std::stringstream error_description;
  error_description << "failed to parse, description:" << description
                    << ", error.line:" << error.line
                    << ", error.description:" << error.description;
  return error_description.str();
}

struct sdp_parse_error : std::runtime_error {
  sdp_parse_error(const std::string &description,
                  const webrtc::SdpParseError &error)
      : std::runtime_error(
            make_sdp_parse_error_description(error, description)) {}
};

class set_remote_description_observer
    : public webrtc::SetRemoteDescriptionObserverInterface {
  logging::logger logger;
  void OnSetRemoteDescriptionComplete(webrtc::RTCError error) override {
    if (error.ok()) {
      BOOST_LOG_SEV(logger, logging::severity::info)
          << "OnSetRemoteDescriptionComplete";
      return;
    }
    std::stringstream out;
    out << "OnSetRemoteDescriptionComplete, error:" << error.message();
    BOOST_LOG_SEV(logger, logging::severity::error) << out.str();
    throw std::runtime_error(out.str());
  }
};
class create_session_description_observer
    : public webrtc::CreateSessionDescriptionObserver {
public:
  create_session_description_observer(
      signaling &signaling_,
      rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_,
      const bool offering)
      : signaling_(signaling_),
        peer_connection(peer_connection_), offering{offering} {
    observer = new rtc::RefCountedObject<set_session_description_observer>(
        "SetLocalDescription");
  }
  ~create_session_description_observer() override {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "~create_session_description_observer";
  }

  void OnSuccess(webrtc::SessionDescriptionInterface *description) override {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "create_session_description_observer::OnSuccess, "
           "description->type():"
        << description->type();
    peer_connection->SetLocalDescription(observer, description);

    std::string sdp;
    description->ToString(&sdp);
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "local description, sdp: " << sdp;
    if (offering) {
      signaling_.send_offer(sdp);
      return;
    }
    signaling_.send_answer(sdp);
  }
  void OnFailure(const std::string &error) override {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "create_session_description_observer::OnFailure";
    throw std::runtime_error(error);
  }

  signaling &signaling_;
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection;
  rtc::scoped_refptr<set_session_description_observer> observer;
  const bool offering;
  logging::logger logger;
};

} // namespace

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
  // https://webrtc.googlesource.com/src/+/refs/heads/master/examples/peerconnection/client/conductor.cc
  // https://github.com/brkho/client-server-webrtc-example/blob/master/server/src/main.cpp
  // https://github.com/llamerada-jp/webrtc-cpp-sample/blob/master/main.cpp
  // native "documentation" https://webrtc.org/native-code/native-apis/
  // about security http://webrtc-security.github.io/
  // https://mediasoup.org
  // v69 fixes: https://bugs.chromium.org/p/webrtc/issues/detail?id=9528
  // - https://webrtc-review.googlesource.com/c/src/+/89002/4/audio/BUILD.gn
  // - https://webrtc-review.googlesource.com/c/src/+/89062/4/BUILD.gn

  const bool do_offer = false;
  //  std::cerr << "hello wbrtc, do_offer:" << std::boolalpha << do_offer
  //            << std::endl;
  // rtc::FlagList::SetFlagsFromCommandLine(&argc, argv, true);
  // rtc::FlagList::Print(nullptr, true);

  logging::logger logger;
  rtc::InitializeSSL();
  constexpr bool use_webrtc_threads{/*true*/};

  std::unique_ptr<rtc::Thread> network_thread;
  std::unique_ptr<rtc::Thread> worker_thread;
  std::unique_ptr<rtc::Thread> signaling_thread;
  if (use_webrtc_threads) {
    network_thread = rtc::Thread::CreateWithSocketServer();
    worker_thread = rtc::Thread::Create();
    signaling_thread = rtc::Thread::Create();
    network_thread->Start();
    worker_thread->Start();
    signaling_thread->Start();
  }

  rtc::scoped_refptr<webrtc::AudioDeviceModule> default_adm;
  rtc::scoped_refptr<webrtc::AudioMixer> audio_mixer;
  rtc::scoped_refptr<webrtc::AudioProcessing> audio_processing;
  rtc::scoped_refptr<webrtc::AudioDecoderFactory> audio_decoder_factory =
      webrtc::CreateBuiltinAudioDecoderFactory();
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>
      peer_connection_factory = webrtc::CreatePeerConnectionFactory(
          network_thread.get(), worker_thread.get(), signaling_thread.get(),
          default_adm, webrtc::CreateBuiltinAudioEncoderFactory(),
          audio_decoder_factory, webrtc::CreateBuiltinVideoEncoderFactory(),
          webrtc::CreateBuiltinVideoDecoderFactory(), audio_mixer,
          audio_processing);

  if (!peer_connection_factory)
    throw std::runtime_error("!peer_connection_factory");

  webrtc::PeerConnectionInterface::RTCConfiguration configuration;
  configuration.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
  webrtc::PeerConnectionInterface::IceServer ice_server;
  ice_server.uri = "stun:stun.l.google.com:19302";
  // configuration.servers.push_back(ice_server);
  // should not be done! for testing purposes only! I suddently had to set it.
  // sctp, dtls https://www.tutorialspoint.com/webrtc/webrtc_sctp
  // configuration.enable_dtls_srtp.emplace(false);

  signaling signaling_client;

  data_channel_observer data_channel_observer_;
  peer_connection_observer observer{signaling_client, data_channel_observer_};
  webrtc::PeerConnectionDependencies peer_dependencies{&observer};

  BOOST_LOG_SEV(logger, logging::severity::info)
      << "configuration.servers.size():" << configuration.servers.size();

  [[maybe_unused]] const bool has_value =
      configuration.ice_check_interval_weak_connectivity.has_value();

  rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection =
      peer_connection_factory->CreatePeerConnection(
          configuration, std::move(peer_dependencies));
  if (!peer_connection)
    throw std::runtime_error("!peer_connection");
  rtc::scoped_refptr<set_remote_description_observer>
      set_remote_description_observer_ =
          new rtc::RefCountedObject<set_remote_description_observer>();

  rtc::scoped_refptr<create_session_description_observer>
      create_session_description_observer_ =
          new rtc::RefCountedObject<create_session_description_observer>(
              signaling_client, peer_connection, do_offer);

  rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel;
  if (do_offer) {
#if 0
    signaling_client.on_logged_in.connect([&]() {
      BOOST_LOG_SEV(logger, logging::severity::info) << "creating offer";
      data_channel = peer_connection->CreateDataChannel("funny", nullptr);
      data_channel_observer_.data_channel = data_channel;
      data_channel->RegisterObserver(&data_channel_observer_);
      const webrtc::PeerConnectionInterface::RTCOfferAnswerOptions
          offer_options;
      peer_connection->CreateOffer(create_session_description_observer_,
                                   offer_options);
    });
#endif
  }

#if 0
  signaling_client.on_message.connect([&](const nlohmann::json &message) {
    const auto type = message["type"];
    if (type == "offer") {
      const std::string sdp = message["offer"]["sdp"];
      webrtc::SdpParseError error;
      std::cerr << "we got us an offer, setting sdp:" << sdp << std::endl;
      std::unique_ptr<webrtc::SessionDescriptionInterface> session_description(
          webrtc::CreateSessionDescription(webrtc::SdpType::kOffer, sdp,
                                           &error));
      if (!session_description)
        throw sdp_parse_error("failed to parse sdp", error);
      // `SetRemoteDescription` takes ownership of `session_description`
      std::cerr << "session_description->type():" << session_description->type()
                << std::endl;
      peer_connection->SetRemoteDescription(std::move(session_description),
                                            set_remote_description_observer_);
      const webrtc::PeerConnectionInterface::RTCOfferAnswerOptions
          answer_options;
      std::cerr << "creating answer" << std::endl;
      peer_connection->CreateAnswer(create_session_description_observer_,
                                    answer_options);
    }
    if (type == "answer") {
      const std::string sdp = message["answer"]["sdp"];
      webrtc::SdpParseError error;
      std::cerr << "we got us an answer, setting sdp:" << sdp << std::endl;
      std::unique_ptr<webrtc::SessionDescriptionInterface> session_description(
          webrtc::CreateSessionDescription(webrtc::SdpType::kAnswer, sdp,
                                           &error));
      if (!session_description)
        throw sdp_parse_error("failed to parse sdp", error);
      // `SetRemoteDescription` takes ownership of `session_description`
      std::cerr << "session_description->type():" << session_description->type()
                << std::endl;
      peer_connection->SetRemoteDescription(std::move(session_description),
                                            set_remote_description_observer_);
    }
    if (type == "candidate") {
      std::cerr << "we got us a candidate" << std::endl;
      const auto candidate_json = message["candidate"];
      if (candidate_json.find("sdpMid") == candidate_json.cend())
        return;
      const auto canditate = candidate_json["candidate"];
      const std::string canditate_string = canditate;
      if (canditate_string.empty())
        return;
      const auto sdpMid = candidate_json["sdpMid"];
      const auto sdpMLineIndex = candidate_json["sdpMLineIndex"];
      // const auto usernameFragment = candidate_json["usernameFragment"];
      webrtc::SdpParseError error;
      auto candidate_parsed = webrtc::CreateIceCandidate(
          sdpMid, sdpMLineIndex, canditate_string, &error);
      if (!candidate_parsed)
        throw sdp_parse_error("failed to parse ice-candidate", error);
      const bool success = peer_connection->AddIceCandidate(candidate_parsed);
      if (!success)
        throw std::runtime_error("peer_connection->AddIceCandidate failed");
    }
  });
#endif

  BOOST_LOG_SEV(logger, logging::severity::info)
      << "this is the end as we know it";
}
