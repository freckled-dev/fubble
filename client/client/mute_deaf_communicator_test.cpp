#include "client/add_audio_to_connection.hpp"
#include "client/audio_tracks_volume.hpp"
#include "client/mute_deaf_communicator.hpp"
#include "client/test_client.hpp"
#include "temporary_room/server/test_server.hpp"
#include "utils/uuid.hpp"
#include <gtest/gtest.h>

namespace {
struct test_mute_deaf_client {
  test_mute_deaf_client(test_executor &executors,
                        const client::testing::test_client::connect_information
                            &connect_information_,
                        const std::string &room_name)
      : executors(executors),
        connect_information_{connect_information_}, room_name{room_name} {}

  test_executor &executors;
  client::testing::test_client::connect_information connect_information_;
  const std::string room_name;
  client::testing::test_client client{executors, connect_information_,
                                      room_name};
  std::shared_ptr<client::add_audio_to_connection> audio_track_adder =
      client::add_audio_to_connection::create_noop();
  std::shared_ptr<client::audio_tracks_volume> audio_tracks_volume =
      client::audio_tracks_volume::create(*client.rooms, client.tracks_adder,
                                          audio_track_adder,
                                          *client.own_audio_track);
  std::unique_ptr<client::mute_deaf_communicator> mute_deaf_communicator =
      client::mute_deaf_communicator::create(client.rooms, audio_tracks_volume);
};
struct MuteDeafCommunicator : testing::Test {
  MuteDeafCommunicator() {
    temporary_room_server.application->run();
    client::testing::test_client::connect_information connect_information_{
        temporary_room_server.make_http_server_and_fields()};
    const auto room_name = uuid::generate();
    first = std::make_unique<test_mute_deaf_client>(
        executors, connect_information_, room_name);
    second = std::make_unique<test_mute_deaf_client>(
        executors, connect_information_, room_name);
  }

  test_executor executors;
  temporary_room::testing::server temporary_room_server{executors.context};
  std::unique_ptr<test_mute_deaf_client> first;
  std::unique_ptr<test_mute_deaf_client> second;
};
} // namespace

#if 0
TEST_F(MuteDeafCommunicator, SetUp) {}
#endif

TEST_F(MuteDeafCommunicator, Deafen) {
  auto first_done = first->client.join("first").then(
      executors.inline_executor, [&](auto result) {
        result.get();
        first->audio_tracks_volume->deafen(true);
      });
  second->mute_deaf_communicator->on_deafed.connect(
      [this]([[maybe_unused]] auto id, bool deafed) {
        EXPECT_TRUE(deafed);
        executors.context.stop();
      });
  auto second_done = second->client.join("second").then(
      executors.inline_executor, [&](auto result) { result.get(); });
  executors.run_context();
  first_done.get();
  second_done.get();
}
