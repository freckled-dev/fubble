#include "client/add_audio_to_connection.hpp"
#include "client/audio_tracks_volume.hpp"
#include "client/mute_deaf_communicator.hpp"
#include "client/test_client.hpp"
#include "temporary_room/server/test_server.hpp"
#include "utils/uuid.hpp"
#include <gtest/gtest.h>

namespace {
struct MuteDeafCommunicator : testing::Test {
  test_executor executors;
  temporary_room::testing::server temporary_room_server{executors.context};
  client::testing::test_client::connect_information connect_information_{
      temporary_room_server.make_http_server_and_fields()};
  client::testing::test_client client{executors, connect_information_,
                                      uuid::generate()};
  std::shared_ptr<client::add_audio_to_connection> audio_track_adder =
      client::add_audio_to_connection::create_noop();
  std::shared_ptr<client::audio_tracks_volume> audio_tracks_volume =
      client::audio_tracks_volume::create(*client.rooms, client.tracks_adder,
                                          audio_track_adder,
                                          *client.own_audio_track);
  std::unique_ptr<client::mute_deaf_communicator> mute_deaf_communicator =
      client::mute_deaf_communicator::create(client.rooms, audio_tracks_volume);
};
} // namespace

TEST_F(MuteDeafCommunicator, SetUp) {}

TEST_F(MuteDeafCommunicator, Deafen) {
  auto done = client.join(uuid::generate())
                  .then(executors.inline_executor, [&](auto result) {
                    result.get();
                    audio_tracks_volume->deafen(true);
                  });
  executors.context.run();
  done.get();
}
