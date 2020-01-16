#include "client.hpp"
#include "client_connector.hpp"
#include "utils/executor_asio.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <gtest/gtest.h>

TEST(Client, Instance) {
  boost::asio::io_context io_context;
  // executor_asio executor{io_context};
  boost::asio::executor executor = io_context.get_executor();
  session::client client{executor};
}

TEST(Client, Connect) {
  boost::asio::io_context io_context;
  boost::asio::executor asio_executor = io_context.get_executor();
  boost::inline_executor executor;
  session::client client{asio_executor};
  session::client_connector connector{client};
  auto connected = connector.connect();
  bool called{};
  connected.then(executor, [&](auto) {
    called = true;
    io_context.stop();
  });
  io_context.run();
  EXPECT_TRUE(called);
}
