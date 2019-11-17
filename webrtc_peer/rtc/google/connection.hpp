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
  boost::future<void>
  set_local_description(const session_description &) override;
  boost::future<void>
  set_remote_description(const session_description &) override;
  void add_track(track_ptr) override;

protected:
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

private:
  logging::logger logger;
  rtc::scoped_refptr<::webrtc::PeerConnectionInterface> native;
};
} // namespace rtc::google

#endif
