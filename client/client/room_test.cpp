#include "executor_asio.hpp"
#include "joiner.hpp"
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
  websocket::connector websocket_connector{context,
                                           websocket_connection_creator};

  signalling::json_message signalling_json;
  signalling::client::connection_creator signalling_connection_creator{
      context, boost_executor, signalling_json};
  signalling::client::client::connect_information connect_information{
      "localhost", "8000"};
  signalling::client::client_creator client_creator{
      boost_executor, websocket_connector, signalling_connection_creator,
      connect_information};

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

#if 0
TEST_F(Room, TwoJoins) {
  auto joined = join("some name");
  joined.then(boost_executor, [&](auto room) {
    EXPECT_TRUE(room.has_value());
    called = true;
    context.stop();
  });
  context.run();
  EXPECT_TRUE(called);
}
#endif

