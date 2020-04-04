#include "add_data_channel.hpp"
#include "executor_asio.hpp"
#include "joiner.hpp"
#include "matrix/testing.hpp"
#include "own_media.hpp"
#include "own_participant.hpp"
#include "participant_creator_creator.hpp"
#include "peer_creator.hpp"
#include "room.hpp"
#include "room_creator.hpp"
#include "rooms.hpp"
#include "rtc/data_channel.hpp"
#include "rtc/google/asio_signalling_thread.hpp"
#include "rtc/google/factory.hpp"
#include "session/client_connector.hpp"
#include "session/room_joiner.hpp"
#include "signalling/client/client_creator.hpp"
#include "signalling/client/connection_creator.hpp"
#include "signalling/json_message.hpp"
#include "temporary_room/testing.hpp"
#include "tracks_adder.hpp"
#include "uuid.hpp"
#include "wait_for_event.hpp"
#include "websocket/connection_creator.hpp"
#include "websocket/connector.hpp"
#include <gtest/gtest.h>

namespace {
struct Room : testing::Test {
  logging::logger logger{"Room"};
  boost::asio::io_context context;
  boost::asio::executor executor{context.get_executor()};
  boost::executor_adaptor<executor_asio> boost_executor{
      context}; // TODO remove!
  rtc::google::asio_signalling_thread rtc_signalling_thread{context};
  std::string room_name = uuid::generate();
};
struct test_client {
  // TODO remove `room_name`. it's in the fixture
  test_client(Room &fixture, const std::string &room_name)
      : context(fixture.context),
        room_name(room_name), rtc_connection_creator{
                                  fixture.rtc_signalling_thread.get_native()} {}

  boost::asio::io_context &context;
  std::string room_name;
  boost::asio::executor executor{context.get_executor()};
  boost::executor_adaptor<executor_asio> boost_executor{
      context}; // TODO remove!
  websocket::connection_creator websocket_connection_creator{context};
  websocket::connector_creator websocket_connector{
      context, websocket_connection_creator};

  // signalling
  signalling::json_message signalling_json;
  signalling::client::connection_creator signalling_connection_creator{
      context, boost_executor, signalling_json};
  signalling::client::client::connect_information connect_information{
      "localhost", "8000"};
  signalling::client::client_creator client_creator{
      websocket_connector, signalling_connection_creator, connect_information};

  // session and matrix
  http::client_factory http_client_factory{
      context, matrix::testing::make_http_server_and_fields()};
  http::client http_client_temporary_room{
      context, temporary_room::testing::make_http_server_and_fields()};
  temporary_room::net::client temporary_room_client{http_client_temporary_room};
  matrix::factory matrix_factory;
  matrix::client_factory matrix_client_factory{matrix_factory,
                                               http_client_factory};
  matrix::authentification matrix_authentification{http_client_factory,
                                                   matrix_client_factory};
  session::client_factory client_factory;
  session::client_connector session_connector{client_factory,
                                              matrix_authentification};
  session::room_joiner session_room_joiner{temporary_room_client};

  // rtc
  rtc::google::factory rtc_connection_creator;
  client::peer_creator peer_creator{boost_executor, client_creator,
                                    rtc_connection_creator};

  // client
  client::rooms rooms;
  client::tracks_adder tracks_adder;
  client::own_media own_media;
  client::participant_creator_creator participant_creator_creator{
      peer_creator, tracks_adder, own_media};
  client::room_creator client_room_creator{participant_creator_creator};
  client::joiner joiner{client_room_creator, rooms, session_connector,
                        session_room_joiner};

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
    if (static_cast<int>(room.get_participants().size()) !=
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

#if 1
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
    auto participants = test.room->get_participants();
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
            EXPECT_EQ(3,
                      static_cast<int>(first.room->get_participants().size()));
            EXPECT_EQ(3,
                      static_cast<int>(second.room->get_participants().size()));
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
            EXPECT_EQ(4,
                      static_cast<int>(first.room->get_participants().size()));
            EXPECT_EQ(4,
                      static_cast<int>(second.room->get_participants().size()));
            EXPECT_EQ(4,
                      static_cast<int>(third.room->get_participants().size()));
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
} // namespace

TEST_F(Room, DataChannel) {
  std::unique_ptr<two_participants> participants =
      std::make_unique<two_participants>(*this, room_name);
  client::add_data_channel data_channel;
  participants->client_first.tracks_adder.add(data_channel);
  auto joined = participants->join();
  bool called{};
  auto close = [&] {
    BOOST_LOG_SEV(logger, logging::severity::trace) << "test close";
    called = true;
    participants.reset();
    context.stop();
  };
  auto channel_opened = [&]() {
    BOOST_LOG_SEV(logger, logging::severity::trace) << "test channel_opened";
    boost::asio::post(context, close);
  };
  data_channel.on_added.connect([&](rtc::data_channel_ptr channel) {
    channel->on_opened.connect(channel_opened);
  });
  context.run();
  BOOST_LOG_SEV(logger, logging::severity::trace) << "end run";
  joined.get();
}
#endif
