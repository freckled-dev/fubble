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
  void run_io_contect() {
    io_context.run();
    io_context.restart();
  }
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
  const std::string name;

  joined_client(boost::asio::executor &executor, std::string room_id,
                std::string name)
      : client{executor}, room_id(room_id), name(name) {}

  boost::future<session::room_joiner::room_ptr> operator()() {
    connector.connect().then(
        executor, [this](auto connected) { on_connected(connected); });
    return join_promise.get_future();
  }
  void on_connected(boost::future<void> &connected) {
    EXPECT_TRUE(connected.has_value());
    client.set_name(name).then(executor,
                               [this](auto done) { on_name_set(done); });
  }
  void on_name_set(boost::future<void> &done) {
    EXPECT_TRUE(done.has_value());
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
  joined_client join{asio_executor, room_id, "name"};
  join().then(executor, [&](auto room_future) {
    EXPECT_TRUE(room_future.get()->get_participants().empty());
    io_context.stop();
  });
  io_context.run();
}

namespace {
struct participants_waiter {
  session::room &room;
  int wait_until{2};
  boost::promise<void> promise;
  boost::signals2::scoped_connection joins;

  participants_waiter(session::room &room) : room(room) {}
  boost::future<void> operator()() {
    joins = room.on_joins.connect([this](auto) { check_if_enough(); });
    check_if_enough();
    return promise.get_future();
  }
  void check_if_enough() {
    if (static_cast<int>(room.get_participants().size()) != wait_until)
      return;
    joins.disconnect();
    promise.set_value();
  }
};
} // namespace

TEST_F(Room, TwoJoins) {
  const auto room_id = uuid::generate();
  joined_client first{asio_executor, room_id, "first"};
  joined_client second{asio_executor, room_id, "second"};
  boost::when_all(first(), second()).then(executor, [&](auto rooms_future) {
    // rooms_future is a future of a tuple of 2 futures
    // https://www.boost.org/doc/libs/1_72_0/doc/html/thread/synchronization.html#thread.synchronization.futures.reference.when_all
    auto rooms = rooms_future.get();
    session::room_joiner::room_ptr first_room = std::get<0>(rooms).get();
    session::room_joiner::room_ptr second_room = std::get<1>(rooms).get();
    EXPECT_TRUE(first_room);
    EXPECT_TRUE(second_room);
    io_context.stop();
  });
  io_context.run();
}

namespace {
struct two_in_a_room {};
} // namespace

TEST_F(Room, TwoJoinSignals) {
  const std::string room_id = uuid::generate();
  joined_client first{asio_executor, room_id, "first"};
  joined_client second{asio_executor, room_id, "second"};
  std::unique_ptr<session::room> first_room;
  std::unique_ptr<session::room> second_room;
  boost::when_all(first(), second()).then(executor, [&](auto rooms_future) {
    auto rooms = rooms_future.get();
    first_room = std::get<0>(rooms).get();
    second_room = std::get<1>(rooms).get();
    io_context.stop();
  });
  run_io_contect();
  ASSERT_TRUE(first_room);
  ASSERT_TRUE(second_room);
  participants_waiter first_waiter{*first_room};
  participants_waiter second_waiter{*second_room};
  boost::when_all(first_waiter(), second_waiter()).then(executor, [&](auto) {
    io_context.stop();
  });
  io_context.run();
}

TEST_F(Room, Name) {
  const auto room_id = uuid::generate();
  const std::string name = "fun name";
  joined_client first{asio_executor, room_id, name};
  std::unique_ptr<session::room> first_room;
  first().then(executor, [&](auto room_future) {
    first_room = room_future.get();
    io_context.stop();
  });
  run_io_contect();
  participants_waiter first_waiter{*first_room};
  first_waiter.wait_until = 1;
  first_waiter().then(executor, [&](auto) { io_context.stop(); });
  run_io_contect();
  if (first_room->get_participants().front().name == name)
    return;
  first_room->on_updates.connect([&](auto updatees) {
    EXPECT_EQ(static_cast<int>(updatees.size()), 1);
    EXPECT_EQ(updatees.front().id, first_room->own_id());
    EXPECT_EQ(updatees.front().name, name);
    EXPECT_EQ(first_room->get_participants().front().name, name);
    io_context.stop();
  });
  run_io_contect();
}

TEST_F(Room, DisconnectSignal) {
  // TODO
}
