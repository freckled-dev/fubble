#include "logging/initialser.hpp"
#include <gtest/gtest.h>
#include <nakama-cpp/Nakama.h>

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  Nakama::NLogger::initWithConsoleSink(Nakama::NLogLevel::Debug);
  logging::add_console_log();

  return RUN_ALL_TESTS();
}

