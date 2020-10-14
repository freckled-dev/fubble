#ifndef UUID_E783A63B_9395_477D_8767_422C504840CC
#define UUID_E783A63B_9395_477D_8767_422C504840CC

#include "client/factory.hpp"
#include "client/joiner.hpp"
#include "client/loopback_audio.hpp"
#include "client/own_audio_track.hpp"
#include "client/own_media.hpp"
#include "client/own_participant.hpp"
#include "client/participant_creator_creator.hpp"
#include "client/participants.hpp"
#include "client/peer_creator.hpp"
#include "client/room.hpp"
#include "client/room_creator.hpp"
#include "client/rooms.hpp"
#include "client/tracks_adder.hpp"
#include "http/action_factory.hpp"
#include "http/client_factory.hpp"
#include "http/connection_creator.hpp"
#include "matrix/authentification.hpp"
#include "matrix/client_factory.hpp"
#include "matrix/factory.hpp"
#include "matrix/testing.hpp"
#include "rtc/data_channel.hpp"
#include "rtc/google/asio_signalling_thread.hpp"
#include "rtc/google/factory.hpp"
#include "signalling/client/client.hpp"
#include "signalling/client/connection_creator.hpp"
#include "signalling/json_message.hpp"
#include "signalling/testing.hpp"
#include "temporary_room/net/client.hpp"
#include "temporary_room/server/application.hpp"
#include "temporary_room/testing.hpp"
#include "test_executor.hpp"
#include "utils/executor_asio.hpp"
#include "websocket/connection_creator.hpp"
#include "websocket/connector.hpp"

namespace client::testing {
struct test_client {
  struct connect_information {
    std::pair<http::server, http::fields> temporary_room_ =
        temporary_room::testing::make_http_server_and_fields();
  };

  test_client(test_executor &fixture, const connect_information &information_,
              const std::string &room_name)
      : context(fixture.context), connect_information_{information_},
        room_name(room_name), rtc_connection_creator{
                                  rtc::google::settings{},
                                  fixture.rtc_signalling_thread.get_native()} {}

  test_client(test_executor &fixture, const std::string &room_name)
      : context(fixture.context),
        room_name(room_name), rtc_connection_creator{
                                  rtc::google::settings{},
                                  fixture.rtc_signalling_thread.get_native()} {}

  boost::asio::io_context &context;
  const connect_information connect_information_;
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
  signalling::client::client::connect_information
      signalling_connect_information =
          signalling::testing::make_connect_information();
  signalling::client::factory_impl client_creator{
      websocket_connector, signalling_connection_creator,
      signalling_connect_information};

  // matrix
  std::shared_ptr<http::action_factory> action_factory_ =
      std::make_shared<http::action_factory>(connection_creator_);
  http::client_factory http_client_factory{
      action_factory_, matrix::testing::make_http_server_and_fields()};
  http::client http_client_temporary_room{action_factory_,
                                          connect_information_.temporary_room_};
  temporary_room::net::client temporary_room_client{http_client_temporary_room};
  matrix::factory matrix_factory;
  matrix::client_factory matrix_client_factory{matrix_factory,
                                               http_client_factory};
  matrix::authentification matrix_authentification{http_client_factory,
                                                   matrix_client_factory};

  // rtc
  rtc::google::factory rtc_connection_creator;
  client::peer_creator peer_creator{boost_executor, client_creator,
                                    rtc_connection_creator};

  // client
  client::tracks_adder tracks_adder;
  std::shared_ptr<client::rooms> rooms = std::make_shared<client::rooms>();
  std::unique_ptr<client::own_audio_track> own_audio_track =
      client::own_audio_track::create_noop();
  std::unique_ptr<client::own_video> own_videos_ = client::own_video::create();
  client::own_media own_media{*own_audio_track, *own_videos_};
  client::factory client_factory{context};
  std::shared_ptr<client::desktop_sharing> desktop_sharing_ =
      client::desktop_sharing::create_noop();
  client::participant_creator_creator participant_creator_creator{
      client_factory, peer_creator, tracks_adder, own_media, desktop_sharing_};
  client::room_creator client_room_creator{participant_creator_creator};
  client::joiner joiner{client_room_creator, *rooms, matrix_authentification,
                        temporary_room_client};

  boost::future<std::shared_ptr<client::room>> join(std::string name) {
    client::joiner::parameters join_paramenters;
    join_paramenters.name = name;
    join_paramenters.room = room_name;
    return joiner.join(join_paramenters);
  }
};
} // namespace client::testing

#endif
