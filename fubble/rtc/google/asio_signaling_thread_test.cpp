#include "asio_signaling_thread.hpp"
#include "fubble/utils/joined_thread.hpp"
#include "fubble/utils/wait_for_event.hpp"
#include "test_peer.hpp"
#include <boost/asio/executor.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <gtest/gtest.h>

struct AsioSignalingThread : ::testing::Test {
  logging::logger logger{"AsioSignalingThread"};
  boost::asio::io_context context;
  boost::asio::executor executor{context.get_executor()};
};

TEST_F(AsioSignalingThread, AddDataChannel) {
  rtc::google::asio_signaling_thread signaling_thread{context};
  rtc::google::settings rtc_settings;
  std::shared_ptr<rtc::google::video_encoder_factory_factory>
      video_encoder_factory_factory =
          std::make_shared<rtc::google::video_encoder_factory_factory>();
  rtc::google::factory creator{video_encoder_factory_factory, rtc_settings,
                               signaling_thread.get_native()};
  test_peer peer{creator};
  peer.instance->on_negotiation_needed.connect([&] {
    BOOST_LOG_SEV(logger, logging::severity::debug) << "done";
    context.stop();
  });
  peer.instance->create_data_channel();
  BOOST_LOG_SEV(logger, logging::severity::debug) << "joining context_thread";
  context.run();
}
