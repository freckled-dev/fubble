#include "fubble/utils/logging/initialser.hpp"
#include "log_webrtc_to_logging.hpp"
#include <gtest/gtest.h>

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  logging::add_console_log(logging::severity::debug);
  rtc::google::log_webrtc_to_logging webrtc_logger;

  return RUN_ALL_TESTS();
}
