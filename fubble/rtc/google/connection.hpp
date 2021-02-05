#ifndef RTC_GOOGLE_CONNECTION_HPP
#define RTC_GOOGLE_CONNECTION_HPP

#include "fubble/rtc/connection.hpp"
#include "fubble/rtc/logger.hpp"
#include "video_track_ptr.hpp"
#include <api/candidate.h>
#include <api/media_stream_interface.h>
#include <api/peer_connection_interface.h>

namespace rtc {
namespace google {
// TODO class uses make_shared for tracks instead of extrusive factory
class connection : public rtc::connection,
                   public ::webrtc::PeerConnectionObserver {
public:
  connection();
  ~connection() override;
  void initialise(rtc::scoped_refptr<::webrtc::PeerConnectionInterface> native);
  boost::future<session_description> create_offer(const offer_options& options) override;
  boost::future<session_description> create_answer() override;
  boost::future<void>
  set_local_description(const session_description &) override;
  boost::future<void>
  set_remote_description(const session_description &) override;
  void add_ice_candidate(const ice_candidate &candidate) override;
  void add_track(rtc::track_ptr) override;
  void remove_track(rtc::track_ptr) override;
  // seems like data channels can't be removed!
  rtc::data_channel_ptr create_data_channel() override;
  void close() override;
  void get_stats(const stats_callback& callback) override;

protected:
  void OnConnectionChange(
      webrtc::PeerConnectionInterface::PeerConnectionState new_state) override;
  void OnStandardizedIceConnectionChange(
      webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
  void OnSignalingChange(
      ::webrtc::PeerConnectionInterface::SignalingState new_state) override;
  void
  OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
             const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>
                 &streams) override;
  void OnRemoveTrack(
      rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override;
  void OnDataChannel(::rtc::scoped_refptr<::webrtc::DataChannelInterface>
                         data_channel) override;
  void OnRenegotiationNeeded() override;
  void OnIceGatheringChange(
      webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
  void
  OnIceCandidate(const ::webrtc::IceCandidateInterface *candidate) override;
  ::rtc::track_ptr
  check_handle_video_track(::webrtc::MediaStreamTrackInterface &interface_);
  ::rtc::track_ptr
  check_handle_audio_track(::webrtc::MediaStreamTrackInterface &interface_);
  std::unique_ptr<webrtc::SessionDescriptionInterface>
  cast_session_description(const rtc::session_description &description);

  struct create_session_description_observer
      : webrtc::CreateSessionDescriptionObserver {
    boost::promise<::rtc::session_description> promise;
    session_description result;
    rtc::logger logger{"create_session_description_observer"};
    void OnSuccess(webrtc::SessionDescriptionInterface *description) override;
    void OnFailure(webrtc::RTCError error) override;
  };
  struct set_session_description_observer
      : public webrtc::SetSessionDescriptionObserver {
    boost::promise<void> promise;
    rtc::logger logger{"set_session_description_observer"};
    void OnSuccess() override;
    void OnFailure(webrtc::RTCError error) override;
  };
  struct set_remote_description_observer
      : public webrtc::SetRemoteDescriptionObserverInterface {
    boost::promise<void> promise;
    rtc::logger logger{"set_remote_description_observer"};
    void OnSetRemoteDescriptionComplete(webrtc::RTCError error) override;
  };
  struct sending_track {
    track_ptr track_;
    rtc::scoped_refptr<webrtc::RtpSenderInterface> rtp;
  };
  std::vector<sending_track>::iterator find_sending_track(const track_ptr &);
  struct receiving_track {
    track_ptr track_;
    rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp;
  };
  std::vector<receiving_track>::iterator find_receiving_track(
      const rtc::scoped_refptr<webrtc::RtpReceiverInterface> &);

  rtc::logger logger;
  rtc::scoped_refptr<::webrtc::PeerConnectionInterface> native;
  // if we don't save the data_channels, RTCConnection will crash in clode().
  // Because of pure virtual DataChannel.
  // It does not help, if we close the data_channel inside its constructor
  std::vector<data_channel_ptr> data_channels;
  std::vector<sending_track> sending_tracks;
  std::vector<receiving_track> receiving_tracks;
};
} // namespace google
} // namespace rtc

#endif
