#include "client/add_data_channel.hpp"
#include "test_client.hpp"
#include "test_executor.hpp"
#include "utils/uuid.hpp"
#include "utils/wait_for_event.hpp"
#include <gtest/gtest.h>

namespace {
struct Room : test_executor, testing::Test {
  const std::string room_name = uuid::generate();
};
struct participants_waiter {
  client::room &room;
  int wait_until{2};
  boost::promise<void> promise;
  boost::signals2::scoped_connection joins;

  participants_waiter(client::room &room) : room(room) {}
  boost::future<void> wait() {
    joins = room.get_participants().on_added.connect(
        [this](auto) { check_if_enough(); });
    check_if_enough();
    return promise.get_future();
  }
  void check_if_enough() {
    if (static_cast<int>(room.get_participants().get_all().size()) !=
        wait_until + 1) // +1 because of bot
      return;
    joins.disconnect();
    promise.set_value();
  }
};
} // namespace

TEST_F(Room, Instance) {}

TEST_F(Room, Join) {
  test_client test{*this, room_name};
  auto joined = test.join("some name");
  auto done = joined.then(boost_executor, [&](auto room) {
    context.stop();
    std::shared_ptr<client::room> room_ = room.get();
    EXPECT_TRUE(room_);
    // EXPECT_EQ(room_->get_name(), room_name); // TODO not implemented yet
  });
  context.run();
  done.get();
}

namespace {
struct join_and_wait {
  test_client &fixture;
  std::string name;
  int wait_until;
  std::shared_ptr<client::room> room;
  std::unique_ptr<participants_waiter> waiter;

  join_and_wait(test_client &fixture, std::string name, int wait_until = 2)
      : fixture(fixture), name(name), wait_until{wait_until} {}

  boost::future<void> join() {
    return fixture.join(name)
        .then(fixture.boost_executor,
              [&](auto room_) {
                room = room_.get();
                waiter = std::make_unique<participants_waiter>(*room);
                waiter->wait_until = wait_until;
                return waiter->wait();
              })
        .unwrap();
  }
};
} // namespace

TEST_F(Room, Participant) {
  test_client test_client_{*this, room_name};
  join_and_wait test{test_client_, "some_name", 1};
  auto done = test.join().then(boost_executor, [&](auto result) {
    result.get();
    context.stop();
    auto participants = test.room->get_participants().get_all();
    EXPECT_EQ(static_cast<int>(participants.size()), 1 + 1); // +1 for bot
    auto participant = std::find_if(
        participants.cbegin(), participants.cend(),
        [&](auto check) { return check->get_id() == test.room->get_own_id(); });
    EXPECT_NE(participant, participants.cend());
    auto own = dynamic_cast<client::own_participant *>(*participant);
    EXPECT_NE(own, nullptr);
  });
  context.run();
  done.get();
}

TEST_F(Room, TwoParticipants) {
  test_client client_first{*this, room_name};
  join_and_wait first(client_first, "first", 2);
  test_client client_second{*this, room_name};
  join_and_wait second(client_second, "second", 2);
  auto done =
      boost::when_all(first.join(), second.join())
          .then(boost_executor, [&](auto result) {
            context.stop();
            EXPECT_FALSE(result.has_exception());
            EXPECT_EQ(3, static_cast<int>(
                             first.room->get_participants().get_all().size()));
            EXPECT_EQ(3, static_cast<int>(
                             second.room->get_participants().get_all().size()));
          });
  context.run();
  done.get();
}

TEST_F(Room, ThreeParticipants) {
  test_client client_first{*this, room_name};
  join_and_wait first(client_first, "first", 3);
  test_client client_second{*this, room_name};
  join_and_wait second(client_second, "second", 3);
  test_client client_third{*this, room_name};
  join_and_wait third(client_third, "three", 3);
  auto done =
      boost::when_all(first.join(), second.join(), third.join())
          .then(boost_executor, [&](auto result) {
            context.stop();
            result.get();
            EXPECT_EQ(4, static_cast<int>(
                             first.room->get_participants().get_all().size()));
            EXPECT_EQ(4, static_cast<int>(
                             second.room->get_participants().get_all().size()));
            EXPECT_EQ(4, static_cast<int>(
                             third.room->get_participants().get_all().size()));
          });
  context.run();
  done.get();
}

namespace {
struct two_participants {
  boost::inline_executor executor;
  test_client client_first;
  join_and_wait first{client_first, "first", 2};
  test_client client_second;
  join_and_wait second{client_second, "second", 2};

  two_participants(Room &fixture, const std::string &room_name)
      : client_first{fixture, room_name}, client_second{fixture, room_name} {}

  auto join() {
    return boost::when_all(first.join(), second.join())
        .then(executor, [](auto result) {
          auto got = result.get();
          std::get<0>(got).get();
          std::get<1>(got).get();
        });
  }
};
struct two_participants_with_data_channel {
  std::shared_ptr<client::add_data_channel> data_channel =
      std::make_shared<client::add_data_channel>();
  std::unique_ptr<two_participants> participants;
  boost::promise<void> promise;
  boost::inline_executor executor;

  two_participants_with_data_channel(Room &fixture, std::string room_name) {
    participants = std::make_unique<two_participants>(fixture, room_name);
    participants->client_first.tracks_adder.add(data_channel);
    auto channel_opened = [this]() { promise.set_value(); };
    data_channel->on_added.connect(
        [channel_opened](rtc::data_channel_ptr channel) {
          channel->on_opened.connect(channel_opened);
        });
  }
  boost::future<void> do_() {
    participants->join().then(executor, [this](auto result) {
      if (!result.has_exception())
        return;
      promise.set_exception(result.get_exception_ptr());
    });
    return promise.get_future();
  }
};
} // namespace

TEST_F(Room, DataChannel) {
  two_participants_with_data_channel test{*this, room_name};
  auto channel_opened = [&](boost::future<void> result) {
    result.get();
    boost::asio::post(context, [&] { context.stop(); });
  };
  auto done = test.do_().then(boost_executor, std::move(channel_opened));
  context.run();
  done.get();
}

TEST_F(Room, SaneShutdown) {
  two_participants_with_data_channel test{*this, room_name};
  auto &first_room = test.participants->first.room;
  auto &second_room = test.participants->second.room;
  auto done = test.do_().then(boost_executor, [&](auto result) {
    result.get();
    second_room->get_participants().on_removed.connect([&](auto left) {
      EXPECT_EQ(left.front(), first_room->get_own_id());
      second_room->leave().then(inline_executor, [this](auto result) {
        EXPECT_FALSE(result.has_exception());
        context.stop();
      });
    });
    first_room->leave().then(inline_executor, [](auto result) {
      EXPECT_FALSE(result.has_exception());
    });
  });
  context.run();
  done.get();
}
