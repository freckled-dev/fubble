#include "client.hpp"
#include "utils/executor_asio.hpp"
#include <gtest/gtest.h>

TEST(Client, Instance) {
  boost::asio::io_context io_context;
  // executor_asio executor{io_context};
  boost::asio::executor executor = io_context.get_executor();
  // session::client client{executor};
}
