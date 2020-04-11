#include "client.hpp"
#include "client_connector.hpp"
#include "matrix/client.hpp"
#include "matrix/testing.hpp"
#include "room.hpp"
#include "room_joiner.hpp"
#include "temporary_room/testing.hpp"
#include "utils/executor_asio.hpp"
#include "utils/uuid.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <gtest/gtest.h>

namespace {
struct Room : testing::Test {
  boost::asio::io_context io_context;
  boost::inline_executor executor;
  void run_io_contect() {
    io_context.run();
    io_context.reset();
  }
};
template <class future_tuple_type>
void check_when_all_future_worked(future_tuple_type &check) {
  auto got = check.get();
  std::get<0>(got).get();
  std::get<1>(got).get();
}
struct joined_client {
  boost::inline_executor executor;
  boost::asio::io_context &io_context;
  http::client_factory http_client_factory{
      io_context, matrix::testing::make_http_server_and_fields()};
  http::client http_client_temporary_room{
      io_context, temporary_room::testing::make_http_server_and_fields()};
  temporary_room::net::client temporary_room_client{http_client_temporary_room};
  matrix::factory matrix_factory;
  matrix::client_factory matrix_client_factory{matrix_factory,
                                               http_client_factory};
  matrix::authentification matrix_authentification{http_client_factory,
                                                   matrix_client_factory};
  session::client_factory client_factory;
  session::client_connector connector{client_factory, matrix_authentification};
  std::unique_ptr<session::client> client;
  std::unique_ptr<session::room_joiner> joiner;
  std::unique_ptr<session::room> room;
  const std::string room_id;
  const std::string name;

  joined_client(boost::asio::io_context &io_context, std::string room_id,
                std::string name)
      : io_context(io_context), room_id(room_id), name(name) {}

  boost::future<void> join() {
    return connector.connect()
        .then(executor,
              [this](auto result) {
                client = result.get();
                return set_name();
              })
        .unwrap()
        .then(executor,
              [this](auto result) {
                result.get();
                return on_connected();
              })
        .unwrap()
        .then(executor, [this](auto result) { room = result.get(); });
  }

  boost::future<void> set_name() { return client->set_name(name); }

  boost::future<session::room_joiner::room_ptr> on_connected() {
    joiner = std::make_unique<session::room_joiner>(temporary_room_client);
    return joiner->join(*client, room_id);
  }
}; // namespace
} // namespace

TEST_F(Room, Joiner) {
  auto room_id = uuid::generate();
  joined_client join{io_context, room_id, "name"};
  auto joined = join.join().then(executor, [&](auto result) {
    result.get();
    io_context.stop();
  });
  io_context.run();
  joined.get();
  EXPECT_TRUE(join.room->get_participants().empty());
}

namespace {
struct participants_waiter {
  session::room &room;
  int wait_until{3};
  boost::promise<void> promise;
  boost::signals2::scoped_connection joins;

  participants_waiter(session::room &room) : room(room) {}
  boost::future<void> wait() {
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
  joined_client first{io_context, room_id, "first"};
  joined_client second{io_context, room_id, "second"};
  // rooms_future is a future of a tuple of 2 futures
  // https://www.boost.org/doc/libs/1_72_0/doc/html/thread/synchronization.html#thread.synchronization.futures.reference.when_all
  auto joined = boost::when_all(first.join(), second.join())
                    .then(executor, [&](auto rooms_future) {
                      auto rooms = rooms_future.get();
                      std::get<0>(rooms).get();
                      std::get<1>(rooms).get();
                      EXPECT_TRUE(first.room);
                      EXPECT_TRUE(second.room);
                      io_context.stop();
                    });
  io_context.run();
  joined.get();
}

namespace {
struct two_in_a_room {
  const std::string room_id = uuid::generate();
  boost::inline_executor executor;
  joined_client first;
  joined_client second;
  std::unique_ptr<session::room> &first_room = first.room;
  std::unique_ptr<session::room> &second_room = second.room;
  two_in_a_room(boost::asio::io_context &asio_executor)
      : first{asio_executor, room_id, "first"}, second{asio_executor, room_id,
                                                       "second"} {}
  boost::future<void> join() {
    auto first_joined_and_running =
        first.join().then(executor, [this](auto result) {
          result.get();
          first.client->run();
        });
    auto second_joined_and_running =
        second.join().then(executor, [this](auto result) {
          result.get();
          second.client->run();
        });
    return boost::when_all(std::move(first_joined_and_running),
                           std::move(second_joined_and_running))
        .then(executor,
              [this](auto rooms_future) {
                check_when_all_future_worked(rooms_future);
              })
        .then(executor,
              [this](auto result) {
                result.get();
                return on_rooms_set();
              })
        .unwrap();
  }
  boost::future<void> on_rooms_set() {
    auto first_waiter = std::make_shared<participants_waiter>(*first_room);
    auto second_waiter = std::make_shared<participants_waiter>(*second_room);
    return boost::when_all(first_waiter->wait(), second_waiter->wait())
        .then(executor, [this, first_waiter, second_waiter](auto future) {
          check_when_all_future_worked(future);
        });
  }
};
} // namespace

TEST_F(Room, TwoJoinSignals) {
  two_in_a_room two{io_context};
  auto success = two.join().then(executor, [&](auto) { io_context.stop(); });
  run_io_contect();
  success.get();
}

TEST_F(Room, Name) {
  const auto room_id = uuid::generate();
  const std::string user_name = "fun name";
  joined_client first{io_context, room_id, user_name};
  std::unique_ptr<session::room> &first_room = first.room;
  auto joined = first.join().then(executor, [&](auto result) {
    result.get();
    io_context.stop();
  });
  run_io_contect();
  joined.get();
  first.client->run();
  participants_waiter first_waiter{*first_room};
  first_waiter.wait_until = 2;
  first_waiter.wait().then(executor, [&](auto) { io_context.stop(); });
  run_io_contect();
  auto &participants = first_room->get_participants();
  const auto found_name =
      std::find_if(participants.begin(), participants.end(),
                   [&](auto check) { return check.name == user_name; });
  EXPECT_NE(found_name, participants.end());
}

TEST_F(Room, DisconnectSignal) {
  two_in_a_room two{io_context};
  auto leave_check = [&](auto leaves) {
    EXPECT_EQ(static_cast<int>(leaves.size()), 1);
    EXPECT_EQ(leaves.front(), two.second.client->get_id());
    io_context.stop();
  };
  auto done = two.join().then(executor, [&](auto result) {
    result.get();
    two.first_room->on_leaves.connect(leave_check);
    return two.second.client->leave_room(*two.second_room);
  });
  run_io_contect();
  done.get();
}

