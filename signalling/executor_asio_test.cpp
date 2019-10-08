#include "executor_asio.hpp"
#include <boost/thread/executors/generic_executor_ref.hpp>
#include <gtest/gtest.h>

TEST(ExecutorAsio, Submit) {
  boost::asio::io_context context;
  executor_asio executor{context};
  bool called{};
  executor.submit([&] { called = true; });
  EXPECT_FALSE(called);
  EXPECT_EQ(context.run(), std::size_t{1});
  EXPECT_TRUE(called);
}

TEST(ExecutorAsio, GenericExecutorRef) {
  boost::asio::io_context context;
  executor_asio executor_asio_{context};
  boost::generic_executor_ref executor{executor_asio_};
  bool called{};
  executor.submit([&] { called = true; });
  EXPECT_FALSE(called);
  EXPECT_EQ(context.run(), std::size_t{1});
  EXPECT_TRUE(called);
}

TEST(ExecutorAsio, GenericExecutorRefCopy) {
  boost::asio::io_context context;
  executor_asio executor_asio_{context};
  boost::generic_executor_ref executor_to_copy{executor_asio_};
  bool called{};
  {
    boost::generic_executor_ref executor(executor_to_copy);
    executor.submit([&] { called = true; });
  }
  EXPECT_FALSE(called);
  EXPECT_EQ(context.run(), std::size_t{1});
  EXPECT_TRUE(called);
}
