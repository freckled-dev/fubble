#include "asio_signalling_thread.hpp"
#include "joined_thread.hpp"
#include "test_peer.hpp"
#include "wait_for_event.hpp"
#include <boost/asio/executor.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <gtest/gtest.h>

struct AsioSignallingThread : ::testing::Test {
  logging::logger logger;
  boost::asio::io_context context;
  boost::asio::executor executor{context.get_executor()};
  boost::asio::executor_work_guard<boost::asio::executor> work_guard{executor};
  joined_thread context_thread{[this] {
    context.run();
    BOOST_LOG_SEV(logger, logging::severity::debug) << "context.run() over";
  }};
};

TEST_F(AsioSignallingThread, AddDataChannel) {
  // factory immediately posts to context. if context is not running, it will
  // block!
  wait_for_event waiter;
  rtc::google::factory creator{
      std::make_unique<rtc::google::asio_signalling_thread>(context)};
  test_peer peer{creator};
  peer.instance->on_negotiation_needed.connect([&] {
    BOOST_LOG_SEV(logger, logging::severity::debug) << "done";
    work_guard.reset();
    waiter.event();
  });
  peer.instance->create_data_channel();
  BOOST_LOG_SEV(logger, logging::severity::debug) << "joining context_thread";
  // context_thread.join();
  waiter.wait();
}
