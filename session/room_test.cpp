#include "client.hpp"
#include "client_connector.hpp"
#include "room.hpp"
#include "room_joiner.hpp"
#include "utils/executor_asio.hpp"
#include "utils/uuid.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <gtest/gtest.h>

TEST(Room, Joiner) {
  boost::asio::io_context io_context;
  boost::asio::executor asio_executor = io_context.get_executor();
  boost::inline_executor executor;
  session::client client{asio_executor};
  session::client_connector connector{client};
  session::room_joiner joiner = client;
  auto room_id = uuid::generate();
  auto join_callback = [&](boost::future<session::room_joiner::room_ptr> room) {
    EXPECT_TRUE(room.has_value());
    EXPECT_TRUE(room.get());
    io_context.stop();
  };
  connector.connect().then(executor, [&](auto connected) {
    EXPECT_TRUE(connected.has_value());
    joiner.join(room_id).then(executor, std::move(join_callback));
  });
  io_context.run();
}
