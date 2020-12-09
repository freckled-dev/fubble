#include "logging/initialser.hpp"
#include <gtest/gtest.h>
extern "C" {
#include <gst/gst.h>
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  logging::add_console_log();

  GError *error{};
  if (!gst_init_check(&argc, &argv, &error)) {
    logging::logger logger;
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "could not initialise gstreamer, message:" << error->message;
    return 1;
  }
  auto result = RUN_ALL_TESTS();
  gst_deinit();
  return result;
}
