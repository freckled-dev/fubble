#include "connection.hpp"
#include "data_channel.hpp"
#include "factory.hpp"
#include "video_track.hpp"
#include "video_track_source.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <thread>

namespace {
struct GoogleConnection : ::testing::Test {
  logging::logger logger;
  rtc::google::factory creator;
};
struct test_peer {
  boost::inline_executor executor;
  std::unique_ptr<rtc::google::connection> instance;

  test_peer(rtc::google::factory &creator)
      : instance(creator.create_connection()) {}
  ~test_peer() { instance->close(); }

  boost::shared_future<rtc::session_description> create_offer() {
    return instance->create_offer();
  }

  boost::shared_future<rtc::session_description> create_answer() {
    return instance->create_answer();
  }

  boost::future<void> set_local_description(
      boost::shared_future<rtc::session_description> description) {
    return description
        .then(executor,
              [this](auto description) {
                return instance->set_local_description(description.get());
              })
        .unwrap();
  }
  boost::future<void> set_remote_description(
      boost::shared_future<rtc::session_description> description) {
    return description
        .then(executor,
              [this](auto description) {
                return instance->set_remote_description(description.get());
              })
        .unwrap();
  }
  void connect_ice_candidates(test_peer &other) {
    instance->on_ice_candidate.connect(
        [&](auto candidate) { other.instance->add_ice_candidate(candidate); });
    other.instance->on_ice_candidate.connect(
        [&](auto candidate) { instance->add_ice_candidate(candidate); });
  }
};
struct connection {
  test_peer offering;
  test_peer answering;
  connection(rtc::google::factory &creator)
      : offering{creator}, answering{creator} {
    offering.connect_ice_candidates(answering);
  }
  boost::future<void> operator()() {
    auto offer = offering.create_offer();
    offering.set_local_description(offer);
    boost::shared_future<rtc::session_description> answer =
        answering.set_remote_description(offer).then(
            [&](auto) { return answering.create_answer().get(); });
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
#if 0
struct add_video_channel {
  add_video_channel(connection& connection_) {
    rtc::track_ptr track = 
  }
};
#endif
struct wait_for_event {
  std::mutex mutex;
  wait_for_event() { mutex.lock(); }
  void wait() { std::unique_lock<std::mutex> end(mutex); }
  void event() { mutex.unlock(); }
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
      [&] { offer = peer.instance->create_offer(); });
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
  boost::signals2::connection signal_connection =
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
  auto source = std::make_shared<rtc::google::video_track_source>();
  auto track = creator.create_video_track(source);
  EXPECT_TRUE(track);
}

TEST_F(GoogleConnection, VideoTrackInOffer) {
  auto source = std::make_shared<rtc::google::video_track_source>();
  std::shared_ptr<rtc::track> track = creator.create_video_track(source);
  auto connection = creator.create_connection();
  connection->add_track(track);
  auto offer = connection->create_offer();
  auto offer_ = offer.get();
  EXPECT_NE(offer_.sdp.find("video"), std::string::npos);
}

