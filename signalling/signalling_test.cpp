#include "gtest/gtest.h"

TEST(Fun, Add) {
  int result = 4 + 2;
  EXPECT_EQ(result, 6);
  EXPECT_NE(result, 42);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

