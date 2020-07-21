#include "add_data_channel.hpp"
#include "client/factory.hpp"
#include "executor_asio.hpp"
#include "http/action_factory.hpp"
#include "http/client_factory.hpp"
#include "http/connection_creator.hpp"
#include "joiner.hpp"
#include "matrix/authentification.hpp"
#include "matrix/client_factory.hpp"
#include "matrix/factory.hpp"
#include "matrix/testing.hpp"
#include "own_audio.hpp"
#include "own_media.hpp"
#include "own_participant.hpp"
#include "participant_creator_creator.hpp"
#include "participants.hpp"
#include "peer_creator.hpp"
#include "room.hpp"
#include "room_creator.hpp"
#include "rooms.hpp"
#include "rtc/data_channel.hpp"
#include "rtc/google/asio_signalling_thread.hpp"
#include "rtc/google/factory.hpp"
#include "signalling/client/client_creator.hpp"
#include "signalling/client/connection_creator.hpp"
#include "signalling/json_message.hpp"
#include "signalling/testing.hpp"
#include "temporary_room/net/client.hpp"
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
  boost::inline_executor inline_executor;
  rtc::google::asio_signalling_thread rtc_signalling_thread{context};
  std::string room_name = uuid::generate();
};
struct test_client {
  // TODO remove `room_name`. it's in the fixture
  test_client(Room &fixture, const std::string &room_name)
      : context(fixture.context),
        room_name(room_name), rtc_connection_creator{
                                  rtc::google::settings{},
                                  fixture.rtc_signalling_thread.get_native()} {}

  boost::asio::io_context &context;
  http::connection_creator connection_creator_{context};
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
  signalling::client::client::connect_information connect_information =
      signalling::testing::make_connect_information();
  signalling::client::client_creator client_creator{
      websocket_connector, signalling_connection_creator, connect_information};

  // matrix
  http::action_factory action_factory_{connection_creator_};
  http::client_factory http_client_factory{
      action_factory_, matrix::testing::make_http_server_and_fields()};
  http::client http_client_temporary_room{
      action_factory_, temporary_room::testing::make_http_server_and_fields()};
  temporary_room::net::client temporary_room_client{http_client_temporary_room};
  matrix::factory matrix_factory;
  matrix::client_factory matrix_client_factory{matrix_factory,
                                               http_client_factory};
  matrix::authentification matrix_authentification{http_client_factory,
                                                   matrix_client_factory};
  //
  // rtc
  rtc::google::factory rtc_connection_creator;
  client::peer_creator peer_creator{boost_executor, client_creator,
                                    rtc_connection_creator};

  // client
  client::rooms rooms;
  client::tracks_adder tracks_adder;
  client::loopback_audio own_audio_{rtc_connection_creator};
  client::own_media own_media{own_audio_};
  client::factory client_factory{context};
  client::participant_creator_creator participant_creator_creator{
      client_factory, peer_creator, tracks_adder, own_media};
  client::room_creator client_room_creator{participant_creator_creator};
  client::joiner joiner{client_room_creator, rooms, matrix_authentification,
                        temporary_room_client};

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
  std::unique_ptr<two_participants> participants;
  client::add_data_channel data_channel;
  boost::promise<void> promise;
  boost::inline_executor executor;

  two_participants_with_data_channel(Room &fixture, std::string room_name) {
    participants = std::make_unique<two_participants>(fixture, room_name);
    participants->client_first.tracks_adder.add(data_channel);
    auto channel_opened = [this]() { promise.set_value(); };
    data_channel.on_added.connect(
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
