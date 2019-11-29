#ifndef RTC_GOOGLE_CONNECTION_HPP
#define RTC_GOOGLE_CONNECTION_HPP

#include "logging/logger.hpp"
#include "rtc/connection.hpp"
#include <api/candidate.h>
#include <api/peer_connection_interface.h>

namespace rtc::google {
class connection : public rtc::connection,
                   public ::webrtc::PeerConnectionObserver {
public:
  ~connection() override;
  void initialise(rtc::scoped_refptr<::webrtc::PeerConnectionInterface> native);
  boost::future<session_description> create_offer() override;
  boost::future<session_description> create_answer() override;
  boost::future<void>
  set_local_description(const session_description &) override;
  boost::future<void>
  set_remote_description(const session_description &) override;
  void add_ice_candidate(const ice_candidate &candidate) override;
  void add_track(track_ptr) override;
  // seems like data channels can't be removed!
  rtc::data_channel_ptr create_data_channel() override;
  void close() override;

protected:
  void OnConnectionChange(
      webrtc::PeerConnectionInterface::PeerConnectionState new_state) override;
  void OnSignalingChange(
      ::webrtc::PeerConnectionInterface::SignalingState new_state) override;
  void
  OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
             const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>
                 &streams) override;
  void OnDataChannel(::rtc::scoped_refptr<::webrtc::DataChannelInterface>
                         data_channel) override;
  void OnRenegotiationNeeded() override;
  void OnIceGatheringChange(
      webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
  void
  OnIceCandidate(const ::webrtc::IceCandidateInterface *candidate) override;

  struct create_session_description_observer
      : webrtc::CreateSessionDescriptionObserver {
    boost::promise<::rtc::session_description> promise;
    session_description result;
    logging::logger logger;
    void OnSuccess(webrtc::SessionDescriptionInterface *description) override;
    void OnFailure(const std::string &error) override;
  };
  struct set_session_description_observer
      : public webrtc::SetSessionDescriptionObserver {
    boost::promise<void> promise;
    logging::logger logger;
    void OnSuccess() override;
    void OnFailure(const std::string &error) override;
  };

  logging::logger logger;
  rtc::scoped_refptr<::webrtc::PeerConnectionInterface> native;
  std::vector<data_channel_ptr> data_channels;
};
} // namespace rtc::google

#endif
