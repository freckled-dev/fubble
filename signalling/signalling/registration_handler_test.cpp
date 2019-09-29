#include "registration_handler.hpp"
#include "signalling/connection.hpp"
#include "signalling/device/creator.hpp"
#include <boost/thread/executor.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/executors/loop_executor.hpp>
#include <boost/thread/executors/scheduler.hpp>
#include <gtest/gtest.h>

TEST(RegistrationHandler, Setup) {
  signalling::device::creator creator;
  boost::inline_executor executor;
  boost::generic_executor_ref executor_ref{executor};
  signalling::registration_handler handler{executor_ref, creator};
}

struct mock_connection : signalling::connection {
  boost::future<signalling::registration> read_registration() override {
    signalling::registration result;
    result.key = "first";
    return boost::make_ready_future(result);
  }
};

TEST(Future, Then) {
  boost::future fullfilled = boost::make_ready_future();
  bool called{};
  auto future_then = fullfilled.then(boost::launch::deferred, [&](auto value) {
    called = true;
    return value.get();
  });
  EXPECT_FALSE(called);
  future_then.get();
  EXPECT_TRUE(called);
}

TEST(Future, ThenWithPromise) {
  boost::promise<int> promise;
  boost::future future = promise.get_future();
  bool called{};
  auto future_then = future.then(boost::launch::sync, [&](auto value) {
    called = true;
    return value.get();
  });
  promise.set_value(42);
  EXPECT_TRUE(called);
  EXPECT_EQ(future_then.get(), 42);
  EXPECT_TRUE(future_then.has_value());
  EXPECT_TRUE(called);
}

TEST(Executor, InlineExecutor) {
  boost::inline_executor executor;
  bool called{};
  executor.submit([&] { called = true; });
  EXPECT_TRUE(called);
}

TEST(Executor, LoopExecutor) {
  boost::loop_executor executor;
  bool called{};
  executor.submit([&] { called = true; });
  EXPECT_FALSE(called);
  executor.run_queued_closures();
  EXPECT_TRUE(called);
}

TEST(Executor, GenericExecutorRefCopy) {
  boost::inline_executor executor;
  boost::generic_executor_ref generic{executor};
  boost::generic_executor_ref copy{generic};
  bool called{};
  copy.submit([&] { called = true; });
  EXPECT_TRUE(called);
}

TEST(Executor, FutureThenOnExecutor) {
  boost::loop_executor executor;
  bool called{};
  boost::future fullfilled = boost::make_ready_future();
  fullfilled.then(executor, [&](auto value) {
    called = true;
    return value;
  });
  EXPECT_FALSE(called);
  executor.run_queued_closures();
  EXPECT_TRUE(called);
}

TEST(Exetutor, TypeErasure) {
  boost::loop_executor executor;
  boost::generic_executor_ref type_erased(executor);
  bool called{};
  type_erased.submit([&]() { called = true; });
  executor.run_queued_closures();
  EXPECT_TRUE(called);
}

TEST(RegistrationHandler, AddOffering) {
  signalling::device::creator creator;
  boost::inline_executor executor;
  boost::generic_executor_ref executor_ref{executor};
  signalling::registration_handler handler{executor_ref, creator};
  auto connection = std::make_shared<mock_connection>();
  EXPECT_TRUE(handler.get_registered().empty());
  handler.add(connection);
  EXPECT_EQ(handler.get_registered().size(), std::size_t{1});
}

