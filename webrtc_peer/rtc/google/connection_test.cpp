#include "connection.hpp"
#include "connection_creator.hpp"
#include <fmt/format.h>
#include <gtest/gtest.h>

TEST(GoogleConnection, Instance) {
  rtc::google::connection_creator creator;
  std::unique_ptr<rtc::google::connection> instance = creator();
}

