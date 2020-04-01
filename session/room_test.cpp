#include "client.hpp"
#include "client_connector.hpp"
#include "matrix/client.hpp"
#include "room.hpp"
#include "room_joiner.hpp"
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
http::fields make_http_fields(const http::server &server) {
  http::fields http_fields{server};
  http_fields.target_prefix = "/_matrix/client/r0/";
  return http_fields;
}
struct joined_client {
  boost::inline_executor executor;
  boost::asio::io_context &io_context;
  // TODO move the creation of server and fields to
  // ::matrix::testing::make_something
  http::server http_server_matrix{"localhost", "8008"};
  http::fields http_fields_matrix = make_http_fields(http_server_matrix);
  http::client_factory http_client_factory{io_context, http_server_matrix,
                                           http_fields_matrix};
  http::server http_server_temporary_room{"localhost", "8009"};
  http::fields http_fields_temporary_room{http_server_temporary_room};
  http::client http_client_temporary_room{
      io_context, http_server_temporary_room, http_fields_temporary_room};
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
                return on_connected();
              })
        .unwrap()
        .then(executor, [this](auto result) {
          room = result.get();
          return;
        });
  }

  boost::future<session::room_joiner::room_ptr> on_connected() {
    // TODO set name of user
    joiner =
        std::make_unique<session::room_joiner>(temporary_room_client, *client);
    return joiner->join(room_id);
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
    return boost::when_all((*first_waiter)(), (*second_waiter)())
        .then(executor, [this, first_waiter, second_waiter](auto future) {
          check_when_all_future_worked(future);
        });
  }
  boost::future<void> run() {
    return boost::when_all(first.client->run(), second.client->run())
        .then([](auto result) { check_when_all_future_worked(result); });
  }
};
} // namespace

TEST_F(Room, TwoJoinSignals) {
  two_in_a_room two{io_context};
  auto success = two.join().then(executor, [&](auto) {
    //
    io_context.stop();
  });
  run_io_contect();
  success.get();
}
#if 0

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
  two_in_a_room two{asio_executor};
  auto leave_check = [&](auto leaves) {
    EXPECT_EQ(static_cast<int>(leaves.size()), 1);
    EXPECT_EQ(leaves.front(), two.second_room->own_id());
    io_context.stop();
  };
  two().then(executor, [&](auto) {
    two.first_room->on_leaves.connect(leave_check);
    two.second.client.close();
  });
  run_io_contect();
}
#endif
