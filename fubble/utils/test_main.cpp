#include "fubble/utils/logging/initialser.hpp"
#include <gtest/gtest.h>

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  logging::add_console_log(logging::severity::debug);

  return RUN_ALL_TESTS();
}
