#include "logging/logger.hpp"
#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/create_peerconnection_factory.h>
// #include <api/media_stream_interface.h>
#include <api/peer_connection_interface.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>
#include <boost/log/keywords/auto_flush.hpp>
#include <boost/log/keywords/format.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/signals2/signal.hpp>
#include <media/base/adapted_video_track_source.h>
#include <rtc_base/ssl_adapter.h>

// #include <modules/audio_device/include/audio_device.h>
// #include <modules/audio_processing/include/audio_processing.h>

namespace {
class signaling {
public:
  boost::signals2::signal<void(const std::string &candidate,
                               const std::string &sdp_mid,
                               const int sdp_mline_index)>
      on_ice_candidate;
  boost::signals2::signal<void(std::string)> on_offer;
  boost::signals2::signal<void(std::string)> on_answer;
};

class data_channel_observer : public webrtc::DataChannelObserver {
public:
  rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel;
  logging::logger logger;
  bool did_send_echo{};

  void OnStateChange() override {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "datachannel OnStateChange";
    auto state = data_channel->state();
    if (state != webrtc::DataChannelInterface::DataState::kOpen)
      return;
    std::string message{"hello data channel"};
    webrtc::DataBuffer message_wrapper{message};
    BOOST_LOG_SEV(logger, logging::severity::info) << "sending a message";
    data_channel->Send(message_wrapper);
  }
  void OnMessage(const webrtc::DataBuffer &buffer) override {
    const std::string message(buffer.data.data<char>(), buffer.data.size());
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "OnMessage, buffer.binary:" << buffer.binary
        << ", message.size():" << message.size();
    BOOST_LOG_SEV(logger, logging::severity::info) << message;
    if (did_send_echo)
      return;
    did_send_echo = true;
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
    signaling_.on_ice_candidate(candidate_send, sdpMid, sdpMLineIndex);
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
      signaling_.on_offer(sdp);
      return;
    }
    signaling_.on_answer(sdp);
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

// struct video_device : webrtc::VideoTrackSourceInterface {};
struct video_device : rtc::AdaptedVideoTrackSource {
  ~video_device() override = default;
  SourceState state() const override {
    return webrtc::MediaSourceInterface::kLive;
  }
  bool remote() const override {
    // TODO check what this bool indicates
    return true;
  }

  // Indicates that parameters suitable for screencasts should be automatically
  // applied to RtpSenders.
  // TODO(perkj): Remove these once all known applications have moved to
  // explicitly setting suitable parameters for screencasts and don't need this
  // implicit behavior.
  bool is_screencast() const override { return true; }

  // Indicates that the encoder should denoise video before encoding it.
  // If it is not set, the default configuration is used which is different
  // depending on video codec.
  // TODO(perkj): Remove this once denoising is done by the source, and not by
  // the encoder.
  absl::optional<bool> needs_denoising() const override { return {}; }
};

struct connection {
  logging::logger logger;
  signaling &signaling_client;
  data_channel_observer data_channel_observer_;
  peer_connection_observer observer{signaling_client, data_channel_observer_};
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection;
  rtc::scoped_refptr<set_remote_description_observer>
      set_remote_description_observer_;
  rtc::scoped_refptr<create_session_description_observer>
      create_session_description_observer_;
  rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel;
  webrtc::PeerConnectionFactoryInterface &peer_connection_factory;

  connection(signaling &signaling_client,
             webrtc::PeerConnectionFactoryInterface &peer_connection_factory)
      : signaling_client(signaling_client),
        peer_connection_factory(peer_connection_factory) {
    webrtc::PeerConnectionInterface::RTCConfiguration configuration;
    configuration.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
    webrtc::PeerConnectionInterface::IceServer ice_server;
    ice_server.uri = "stun:stun.l.google.com:19302";
    // configuration.servers.push_back(ice_server);
    // should not be done! for testing purposes only! I suddently had to set it.
    // sctp, dtls https://www.tutorialspoint.com/webrtc/webrtc_sctp
    // configuration.enable_dtls_srtp.emplace(false);
    webrtc::PeerConnectionDependencies peer_dependencies{&observer};
    peer_connection = peer_connection_factory.CreatePeerConnection(
        configuration, std::move(peer_dependencies));
    if (!peer_connection)
      throw std::runtime_error("!peer_connection");
    set_remote_description_observer_ =
        new rtc::RefCountedObject<set_remote_description_observer>();
  }
  void offer() {
    bool offering{true};
    BOOST_LOG_SEV(logger, logging::severity::info) << "creating offer";
    {
      create_session_description_observer_ =
          new rtc::RefCountedObject<create_session_description_observer>(
              signaling_client, peer_connection, offering);
      data_channel = peer_connection->CreateDataChannel("funny", nullptr);
      data_channel_observer_.data_channel = data_channel;
      data_channel->RegisterObserver(&data_channel_observer_);
    }

    {
      rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> video_device_ =
          new rtc::RefCountedObject<video_device>();
      rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track_(
          peer_connection_factory.CreateVideoTrack("video_track",
                                                   video_device_.get()));
      // main_wnd_->StartLocalRenderer(video_track_);

      std::string stream_id = "stream_id";
      auto result = peer_connection->AddTrack(video_track_, {stream_id});
      if (!result.ok()) {
        BOOST_LOG_SEV(logger, logging::severity::error)
            << "Failed to add video track to PeerConnection: "
            << result.error().message();
      }
    }

    const webrtc::PeerConnectionInterface::RTCOfferAnswerOptions offer_options;
    peer_connection->CreateOffer(create_session_description_observer_,
                                 offer_options);
  }
  void answer() {
    BOOST_LOG_SEV(logger, logging::severity::info) << "creating answer";
    bool offering{false};
    create_session_description_observer_ =
        new rtc::RefCountedObject<create_session_description_observer>(
            signaling_client, peer_connection, offering);
    const webrtc::PeerConnectionInterface::RTCOfferAnswerOptions answer_options;
    peer_connection->CreateAnswer(create_session_description_observer_,
                                  answer_options);
  }
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

  // rtc::FlagList::SetFlagsFromCommandLine(&argc, argv, true);
  // rtc::FlagList::Print(nullptr, true);

  boost::log::add_common_attributes();
  boost::log::add_console_log(
      std::cout, boost::log::keywords::auto_flush = true,
      boost::log::keywords::format = "%TimeStamp% %Severity% %Message%");

  logging::logger logger;
  BOOST_LOG_SEV(logger, logging::severity::info) << "starting";
  rtc::InitializeSSL();

  std::unique_ptr<rtc::Thread> network_thread;
  std::unique_ptr<rtc::Thread> worker_thread;
  std::unique_ptr<rtc::Thread> signaling_thread;
  network_thread = rtc::Thread::CreateWithSocketServer();
  worker_thread = rtc::Thread::Create();
  signaling_thread = rtc::Thread::Create();
  network_thread->Start();
  worker_thread->Start();
  signaling_thread->Start();

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

  std::mutex wait_for_end;
  wait_for_end.lock();

  signaling signaling_offering;
  connection offering{signaling_offering, *peer_connection_factory};

  signaling signaling_answering;
  connection answering{signaling_answering, *peer_connection_factory};

  signaling_offering.on_offer.connect([&](const auto &sdp) {
    webrtc::SdpParseError error;
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "we got us an offer, setting sdp:" << sdp;
    std::unique_ptr<webrtc::SessionDescriptionInterface> session_description(
        webrtc::CreateSessionDescription(webrtc::SdpType::kOffer, sdp, &error));
    if (!session_description)
      throw sdp_parse_error("failed to parse sdp", error);
    // `SetRemoteDescription` takes ownership of `session_description`
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "session_description->type():" << session_description->type();
    answering.peer_connection->SetRemoteDescription(
        std::move(session_description),
        answering.set_remote_description_observer_);
    answering.answer();
  });

  signaling_answering.on_answer.connect([&](const auto &sdp) {
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
    offering.peer_connection->SetRemoteDescription(
        std::move(session_description),
        offering.set_remote_description_observer_);
  });
  struct on_ice_candidate {
    connection &connection_;
    void operator()(const std::string &candidate, const std::string &sdp_mid,
                    const int sdp_mline_index) {
      logging::logger logger;
      BOOST_LOG_SEV(logger, logging::severity::info) << "we got us a candidate";
      if (candidate.empty())
        return;
      webrtc::SdpParseError error;
      auto candidate_parsed = webrtc::CreateIceCandidate(
          sdp_mid, sdp_mline_index, candidate, &error);
      if (!candidate_parsed)
        throw sdp_parse_error("failed to parse ice-candidate", error);
      const bool success =
          connection_.peer_connection->AddIceCandidate(candidate_parsed);
      if (!success)
        throw std::runtime_error("peer_connection->AddIceCandidate failed");
    }
  };
  on_ice_candidate on_ice_candidate_offering{answering};
  on_ice_candidate on_ice_candidate_answering{offering};
  signaling_offering.on_ice_candidate.connect(on_ice_candidate_offering);
  signaling_answering.on_ice_candidate.connect(on_ice_candidate_answering);

  offering.offer();
  BOOST_LOG_SEV(logger, logging::severity::info) << "wait_for_end.lock()";
  wait_for_end.lock();

  BOOST_LOG_SEV(logger, logging::severity::info)
      << "this is the end as we know it";
}
