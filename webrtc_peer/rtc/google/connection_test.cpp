#include "connection.hpp"
#include "connection_creator.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <thread>

namespace {
struct GoogleConnection : ::testing::Test {
  logging::logger logger;
  boost::inline_executor executor;
  rtc::google::connection_creator creator;
  std::unique_ptr<rtc::google::connection> instance = creator();
};
struct wait_for_event {
  std::mutex mutex;
  wait_for_event() { mutex.lock(); }
  void wait() { std::unique_lock<std::mutex> end(mutex); }
  void event() { mutex.unlock(); }
};
} // namespace

TEST_F(GoogleConnection, Instance) { EXPECT_TRUE(instance); }

TEST_F(GoogleConnection, AddDataChannel) {
  wait_for_event waiter;
  instance->on_negotiation_needed.connect([&] { waiter.event(); });
  instance->create_data_channel();
  waiter.wait();
}

TEST_F(GoogleConnection, Offer) {
  boost::future<::rtc::session_description> offer;
  instance->on_negotiation_needed.connect(
      [&] { offer = instance->create_offer(); });
  instance->create_data_channel();
  const ::rtc::session_description description = offer.get();
  instance->close();
  EXPECT_FALSE(description.sdp.empty());
  EXPECT_EQ(description.type_, ::rtc::session_description::type::offer);
}

