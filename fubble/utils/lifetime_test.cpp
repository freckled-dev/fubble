#include "lifetime.hpp"
#include <gtest/gtest.h>

TEST(Lifetime, Instance) { fubble::lifetime::checker checker; }

TEST(Lifetime, WrapVoid) {
  fubble::lifetime::checker checker;
  bool called{};
  auto call = checker.wrap(std::function<void()>([&called] { called = true; }));
  call();
  EXPECT_TRUE(called);
}

TEST(Lifetime, WrapVoidDestroyed) {
  auto checker = std::make_unique<fubble::lifetime::checker>();
  bool called{};
  auto call = checker->wrap([&called] { called = true; });
  checker.reset();
  call();
  EXPECT_FALSE(called);
}

TEST(Lifetime, WrapOneArg) {
  fubble::lifetime::checker checker;
  int called_arg{};
  std::function<void(int)> call =
      checker.wrap<int>([&called_arg](int arg) { called_arg = arg; });
  call(42);
  EXPECT_EQ(called_arg, 42);
}

TEST(Lifetime, Wrap2TimesVoid) {
  auto checker = std::make_unique<fubble::lifetime::checker>();
  int call_count{};
  auto call0 = checker->wrap([&call_count] { ++call_count; });
  auto call1 = checker->wrap([&call_count] { ++call_count; });
  call0();
  call1();
  EXPECT_EQ(call_count, 2);
}

TEST(Lifetime, Wrap2TimesVoidDestroyed) {
  auto checker = std::make_unique<fubble::lifetime::checker>();
  int call_count{};
  auto call0 = checker->wrap([&call_count] { ++call_count; });
  auto call1 = checker->wrap([&call_count] { ++call_count; });
  checker.reset();
  call0();
  call1();
  EXPECT_EQ(call_count, 0);
}
