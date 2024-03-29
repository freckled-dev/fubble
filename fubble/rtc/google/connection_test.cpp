#include "fubble/rtc/google/audio_devices.hpp"
#include "fubble/rtc/google/data_channel.hpp"
#include "fubble/rtc/google/video_track.hpp"
#include "fubble/rtc/google/video_track_source.hpp"
#include "fubble/utils/wait_for_event.hpp"
#include "test_peer.hpp"
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <thread>

namespace {
struct GoogleConnection : ::testing::Test {
  logging::logger logger{"GoogleConnection"};
  rtc::google::factory creator;
};
struct connection {
  test_peer offering;
  test_peer answering;
  boost::inline_executor executor;
  connection(rtc::google::factory &creator)
      : offering{creator}, answering{creator} {
    offering.connect_ice_candidates(answering);
  }
  boost::future<void> operator()() {
    auto offer = offering.create_offer();
    offering.set_local_description(offer);
    boost::shared_future<void> remote_description_future =
        answering.set_remote_description(offer);
    boost::shared_future<rtc::session_description> answer =
        remote_description_future
            .then(executor, [&](auto) { return answering.create_answer(); })
            .unwrap();
    answering.set_local_description(answer);
    return offering.set_remote_description(answer);
  }
};
struct add_data_channel {
private:
  boost::promise<void> promise_open;
  boost::promise<void> promise_answering;

public:
  rtc::data_channel_ptr offering;
  rtc::data_channel_ptr answering;

  add_data_channel(connection &connection_) {
    offering = connection_.offering.instance->create_data_channel();
    connection_.answering.instance->on_data_channel.connect(
        [this](auto data_channel) {
          answering = data_channel;
          promise_answering.set_value();
        });
  }

  auto operator()() {
    offering->on_opened.connect([this]() { promise_open.set_value(); });
    return boost::when_all(promise_open.get_future(),
                           promise_answering.get_future());
  }
};
} // namespace

TEST_F(GoogleConnection, Instance) { test_peer peer{creator}; }

TEST_F(GoogleConnection, AddDataChannel) {
  test_peer peer{creator};
  wait_for_event waiter;
  peer.instance->on_negotiation_needed.connect([&] { waiter.event(); });
  peer.instance->create_data_channel();
  waiter.wait();
}

TEST_F(GoogleConnection, Offer) {
  test_peer peer{creator};
  boost::future<::rtc::session_description> offer;
  peer.instance->on_negotiation_needed.connect(
      [&] { offer = peer.instance->create_offer({}); });
  peer.instance->create_data_channel();
  const ::rtc::session_description description = offer.get();
  EXPECT_FALSE(description.sdp.empty());
  EXPECT_EQ(description.type_, ::rtc::session_description::type::offer);
}

TEST_F(GoogleConnection, SetLocalDescription) {
  test_peer peer{creator};
  boost::future<void> done;
  peer.instance->on_negotiation_needed.connect(
      [&] { done = peer.set_local_description(peer.create_offer()); });
  peer.instance->create_data_channel();
  done.get();
}

TEST_F(GoogleConnection, IceCandidate) {
  wait_for_event waiter;
  test_peer peer{creator};
  utils::signal::connection signal_connection =
      peer.instance->on_ice_candidate.connect([&](auto candidate) {
        signal_connection.disconnect();
        EXPECT_FALSE(candidate.sdp.empty());
        waiter.event();
      });
  peer.instance->create_data_channel();
  peer.set_local_description(peer.create_offer());
  waiter.wait();
}

TEST_F(GoogleConnection, SetRemoteDescription) {
  test_peer offering{creator};
  test_peer answering{creator};
  offering.instance->create_data_channel();
  boost::future<void> done =
      answering.set_remote_description(offering.create_offer());
  done.get();
}

TEST_F(GoogleConnection, Answer) {
  test_peer offering{creator};
  test_peer answering{creator};
  offering.instance->create_data_channel();
  auto offer = offering.create_offer();
  offering.set_local_description(offer);
  auto answer = answering.set_remote_description(offer).then(
      [&](auto) { return answering.create_answer().get(); });
  auto description = answer.get();
  EXPECT_FALSE(description.sdp.empty());
  EXPECT_EQ(description.type_, ::rtc::session_description::type::answer);
}

TEST_F(GoogleConnection, DataConnection) {
  connection connection_{creator};
  add_data_channel data_channels{connection_};
  auto open = data_channels();
  connection_();
  open.get();
  EXPECT_TRUE(data_channels.answering);
}

TEST_F(GoogleConnection, DataExchange) {
  connection connection_{creator};
  add_data_channel data_channels{connection_};
  auto open = data_channels();
  connection_();
  open.get();
  std::string message{"message"};
  wait_for_event waiter;
  data_channels.answering->on_message.connect([&](auto received) {
    EXPECT_FALSE(received.binary);
    auto verify = received.to_string();
    EXPECT_EQ(verify, message);
    waiter.event();
  });
  data_channels.offering->send(message);
  waiter.wait();
}

TEST_F(GoogleConnection, VideoTrackCreation) {
  auto source = std::make_shared<rtc::google::video_source>();
  auto track = creator.create_video_track(source);
  EXPECT_TRUE(track);
}

TEST_F(GoogleConnection, VideoTrackInOffer) {
  auto source = std::make_shared<rtc::google::video_source>();
  std::shared_ptr<rtc::track> track = creator.create_video_track(source);
  auto connection = creator.create_connection();
  connection->add_track(track);
  auto offer = connection->create_offer({});
  auto offer_ = offer.get();
  EXPECT_NE(offer_.sdp.find("video"), std::string::npos);
}

TEST_F(GoogleConnection, OnVideoTrack) {
  connection connection_{creator};
  auto source = std::make_shared<rtc::google::video_source>();
  std::shared_ptr<rtc::track> track = creator.create_video_track(source);
  connection_.offering.instance->add_track(track);
  wait_for_event waiter;
  connection_.answering.instance->on_video_track_added.connect(
      [&](const auto &track) {
        EXPECT_TRUE(track);
        waiter.event();
      });
  auto connected = connection_();
  connected.get();
  waiter.wait();
}

TEST_F(GoogleConnection, EnumerateAudioDevices) {
  auto audio_devices = creator.get_audio_devices();
  audio_devices->enumerate();
#if 0 // may not work depending on system
  audio_devices.is_microphone_muted();
  audio_devices.is_speaker_muted();
#endif
}
