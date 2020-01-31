#include "executor_asio.hpp"
#include "joiner.hpp"
#include "own_participant.hpp"
#include "participant_creator_creator.hpp"
#include "peer_creator.hpp"
#include "room.hpp"
#include "room_creator.hpp"
#include "rooms.hpp"
#include "rtc/google/factory.hpp"
#include "signalling/client/client_creator.hpp"
#include "signalling/client/connection_creator.hpp"
#include "signalling/json_message.hpp"
#include "uuid.hpp"
#include "websocket/connection_creator.hpp"
#include "websocket/connector.hpp"
#include <gtest/gtest.h>

namespace {
struct Room : testing::Test {
  logging::logger logger;
  boost::asio::io_context context;
  boost::asio::executor executor{context.get_executor()};
  boost::executor_adaptor<executor_asio> boost_executor{context};

  websocket::connection_creator websocket_connection_creator{context};
  websocket::connector_creator websocket_connector{
      context, websocket_connection_creator};

  signalling::json_message signalling_json;
  signalling::client::connection_creator signalling_connection_creator{
      context, boost_executor, signalling_json};
  signalling::client::client::connect_information connect_information{
      "localhost", "8000"};
  signalling::client::client_creator client_creator{
      websocket_connector, signalling_connection_creator, connect_information};

  rtc::google::factory rtc_connection_creator;
  client::peer_creator peer_creator{boost_executor, client_creator,
                                    rtc_connection_creator};
  client::rooms rooms;
  client::participant_creator_creator participant_creator_creator{peer_creator};
  client::room_creator client_room_creator{participant_creator_creator};
  client::joiner joiner{executor, client_room_creator, rooms};
  std::string room_name = uuid::generate();

  boost::future<std::shared_ptr<client::room>> join(std::string name) {
    client::joiner::parameters join_paramenters;
    join_paramenters.name = name;
    join_paramenters.room = room_name;
    return joiner.join(join_paramenters);
  }
};
struct participants_waiter {
  client::room &room;
  int wait_until{2};
  boost::promise<void> promise;
  boost::signals2::scoped_connection joins;

  participants_waiter(client::room &room) : room(room) {}
  boost::future<void> wait() {
    joins =
        room.on_participants_join.connect([this](auto) { check_if_enough(); });
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

TEST_F(Room, Instance) {}

TEST_F(Room, Join) {
  bool called{};
  auto joined = join("some name");
  joined.then(boost_executor, [&](auto room) {
    EXPECT_TRUE(room.has_value());
    called = true;
    context.stop();
    std::shared_ptr<client::room> room_ = room.get();
    EXPECT_EQ(room_->get_name(), room_name);
  });
  context.run();
  EXPECT_TRUE(called);
}

namespace {
struct join_and_wait {
  Room &fixture;
  std::string name;
  int wait_until = 2;
  std::shared_ptr<client::room> room;
  std::unique_ptr<participants_waiter> waiter;

  join_and_wait(Room &fixture, std::string name)
      : fixture(fixture), name(name) {}

  boost::future<void> operator()() {
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
  join_and_wait test(*this, "some name");
  test.wait_until = 1;
  bool called{};
  test().then(boost_executor, [&](auto result) {
    EXPECT_FALSE(result.has_exception());
    called = true;
    context.stop();
    auto &participants = test.room->get_participants();
    EXPECT_EQ(static_cast<int>(participants.size()), 1);
    auto participant = participants.front().get();
    EXPECT_EQ(participant->get_id(), test.room->get_own_id());
    auto own = dynamic_cast<client::own_participant *>(participant);
    EXPECT_NE(own, nullptr);
  });
  context.run();
  EXPECT_TRUE(called);
}

TEST_F(Room, TwoParticipants) {
  join_and_wait first(*this, "first");
  join_and_wait second(*this, "second");
  bool called{};
  boost::when_all(first(), second()).then([&](auto result) {
    called = true;
    context.stop();
    EXPECT_FALSE(result.has_exception());
  });
  context.run();
  EXPECT_TRUE(called);
}

