#include "connection.hpp"
#include "connection_creator.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <thread>

namespace {
struct GoogleConnection : ::testing::Test {
  logging::logger logger;
  rtc::google::connection_creator creator;
};
struct test_peer {
  boost::inline_executor executor;
  std::unique_ptr<rtc::google::connection> instance;

  test_peer(rtc::google::connection_creator &creator) : instance(creator()) {}
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
};
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

