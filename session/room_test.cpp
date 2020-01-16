#include "client.hpp"
#include "client_connector.hpp"
#include "room.hpp"
#include "room_joiner.hpp"
#include "utils/executor_asio.hpp"
#include "utils/uuid.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <gtest/gtest.h>

namespace {
struct Room : testing::Test {
  boost::asio::io_context io_context;
  boost::asio::executor asio_executor = io_context.get_executor();
  boost::inline_executor executor;
};
} // namespace

namespace {
struct joined_client {
  session::client client;
  session::client_connector connector{client};
  session::room_joiner joiner = client;
  boost::inline_executor executor;
  boost::promise<session::room_joiner::room_ptr> join_promise;
  const std::string room_id;

  joined_client(boost::asio::executor &executor, std::string room_id)
      : client{executor}, room_id(room_id) {}

  boost::future<session::room_joiner::room_ptr> operator()() {
    connector.connect().then(
        executor, [this](auto connected) { on_connected(connected); });
    return join_promise.get_future();
  }
  void on_connected(boost::future<void> &connected) {
    EXPECT_TRUE(connected.has_value());
    joiner.join(room_id).then(executor,
                              [this](auto joined) { on_joined(joined); });
  }
  void on_joined(boost::future<session::room_joiner::room_ptr> &room_future) {
    EXPECT_TRUE(room_future.has_value());
    session::room_joiner::room_ptr room = room_future.get();
    EXPECT_TRUE(room);
    EXPECT_EQ(room->get_name(), room_id);
    join_promise.set_value(std::move(room));
  }
};
} // namespace

TEST_F(Room, Joiner) {
  auto room_id = uuid::generate();
  joined_client join{asio_executor, room_id};
  join().then(executor, [&](auto room_future) {
    EXPECT_TRUE(room_future.get()->get_participants().empty());
    io_context.stop();
  });
  io_context.run();
}

namespace {
struct {};
} // namespace

TEST_F(Room, TwoJoins) {
  const auto room_id = uuid::generate();
  joined_client first{asio_executor, room_id};
  joined_client second{asio_executor, room_id};
  auto first_joined = first();
  auto second_joined = second();
  boost::when_all(std::move(first_joined), std::move(second_joined))
      .then(executor, [&](auto) { io_context.stop(); });
  io_context.run();
}
