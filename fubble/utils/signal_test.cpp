#include "signal.hpp"
#include <gtest/gtest.h>

TEST(Signals, Instance0) {
  utils::signal::signal<> signal;
  [[maybe_unused]] utils::signal::scoped_connection conn =
      signal.connect([]() {});
}

TEST(Signals, Instance1) {
  utils::signal::signal<int, float> signal;
  [[maybe_unused]] utils::signal::scoped_connection conn =
      signal.connect([](int, float) {});
}

TEST(Signals, Instance2) {
  utils::signal::signal<int, float> signal;
  [[maybe_unused]] utils::signal::scoped_connection conn =
      signal.connect([](int, float) {});
}
