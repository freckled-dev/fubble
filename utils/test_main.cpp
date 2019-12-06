#include "logging/initialser.hpp"
#include <gtest/gtest.h>

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  logging::add_console_log();

  return RUN_ALL_TESTS();
}
