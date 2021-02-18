#ifndef UUID_E783A63B_9395_477D_8767_422C504840CC
#define UUID_E783A63B_9395_477D_8767_422C504840CC

#include "fubble/client/factory.hpp"
#include "fubble/client/joiner.hpp"
#include "fubble/client/loopback_audio.hpp"
#include "fubble/client/own_audio_track.hpp"
#include "fubble/client/own_media.hpp"
#include "fubble/client/own_participant.hpp"
#include "fubble/client/participant_creator_creator.hpp"
#include "fubble/client/participants.hpp"
#include "fubble/client/peer_creator.hpp"
#include "fubble/client/peers.hpp"
#include "fubble/client/room.hpp"
#include "fubble/client/room_creator.hpp"
#include "fubble/client/rooms.hpp"
#include "fubble/client/tracks_adder.hpp"
#include "fubble/http/action_factory.hpp"
#include "fubble/http/client_factory.hpp"
#include "fubble/http/connection_creator.hpp"
#include "fubble/matrix/authentification.hpp"
#include "fubble/matrix/client_factory.hpp"
#include "fubble/matrix/client_synchronizer.hpp"
#include "fubble/matrix/factory.hpp"
#include "fubble/matrix/testing.hpp"
#include "fubble/rtc/data_channel.hpp"
#include "fubble/rtc/google/asio_signaling_thread.hpp"
#include "fubble/rtc/google/factory.hpp"
#include "fubble/signaling/client/client.hpp"
#include "fubble/signaling/client/connection_creator.hpp"
#include "fubble/signaling/json_message.hpp"
#include "fubble/signaling/testing.hpp"
#include "fubble/temporary_room/net/client.hpp"
#include "fubble/temporary_room/server/application.hpp"
#include "fubble/temporary_room/testing.hpp"
#include "fubble/utils/executor_asio.hpp"
#include "fubble/utils/timer.hpp"
#include "fubble/version/server.hpp"
#include "fubble/version/testing.hpp"
#include "fubble/websocket/connection_creator.hpp"
#include "fubble/websocket/connector.hpp"
#include "test_executor.hpp"

namespace client::testing {
struct test_client {
  struct connect_information {
    std::pair<http::server, http::fields> temporary_room_ =
        temporary_room::testing::make_http_server_and_fields();
    std::pair<http::server, http::fields> version_;
  };

  test_client(test_executor &fixture, const connect_information &information_,
              const std::string &room_name)
      : context(fixture.context), connect_information_{information_},
        room_name(room_name), rtc_connection_creator{
                                  rtc::google::settings{},
                                  fixture.rtc_signaling_thread.get_native()} {
    set_own_media();
  }

  test_client(test_executor &fixture, const std::string &room_name)
      : context(fixture.context),
        room_name(room_name), rtc_connection_creator{
                                  rtc::google::settings{},
                                  fixture.rtc_signaling_thread.get_native()} {
    set_own_media();
  }

  void set_own_media() {
    own_media.set_own_audio_track(own_audio_track);
    own_media.set_own_video(own_videos_);
    own_media.set_desktop_sharing(desktop_sharing_);
  }

  boost::asio::io_context &context;
  const connect_information connect_information_;
  std::string room_name;
  boost::asio::executor executor{context.get_executor()};
  boost::executor_adaptor<executor_asio> boost_executor{
      context}; // TODO remove!
  std::shared_ptr<utils::timer_factory> timer_factory =
      std::make_shared<utils::timer_factory>(context);

  // websocket
  websocket::connection_creator websocket_connection_creator{context};
  websocket::connector_creator websocket_connector{
      context, websocket_connection_creator};

  // http
  http::connection_creator connection_creator_{context};
  std::shared_ptr<http::action_factory> action_factory_ =
      std::make_shared<http::action_factory>(connection_creator_);

  // signaling
  signaling::json_message signaling_json;
  signaling::client::connection_creator signaling_connection_creator{
      context, boost_executor, signaling_json};
  signaling::client::connect_information signaling_connect_information =
      signaling::testing::make_connect_information();
  signaling::client::factory_impl client_creator{websocket_connector,
                                                 signaling_connection_creator,
                                                 signaling_connect_information};

  // version
  std::shared_ptr<http::client> version_http_client =
      std::make_shared<http::client>(action_factory_,
                                     connect_information_.version_);
  std::shared_ptr<version::getter> version_getter =
      version::getter::create(version_http_client);

  // matrix
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
  std::shared_ptr<matrix::client_synchronizer> matrix_client_synchronizer =
      matrix::client_synchronizer::create_retrying(
          timer_factory->create_one_shot_timer(std::chrono::seconds(1)));

  // rtc
  rtc::google::factory rtc_connection_creator;
  client::peer_creator peer_creator{boost_executor, client_creator,
                                    rtc_connection_creator,
                                    client::peer::config{}};

  // client
  client::tracks_adder tracks_adder;
  std::shared_ptr<client::rooms> rooms = std::make_shared<client::rooms>();
  std::shared_ptr<client::own_audio_track> own_audio_track =
      client::own_audio_track::create_noop();
  std::shared_ptr<client::own_video> own_videos_ = client::own_video::create();
  std::shared_ptr<client::desktop_sharing> desktop_sharing_ =
      client::desktop_sharing::create_noop();
  client::own_media own_media;
  client::factory client_factory;
  std::shared_ptr<peers> peers_ = std::make_shared<peers>();
  client::participant_creator_creator participant_creator_creator{
      client_factory, peers_, peer_creator, tracks_adder, own_media};
  client::room_creator client_room_creator{participant_creator_creator};
  client::joiner joiner{client_room_creator,     *rooms,
                        matrix_authentification, temporary_room_client,
                        version_getter,          matrix_client_synchronizer};

  boost::future<std::shared_ptr<client::room>> join(std::string name) {
    client::joiner::parameters join_paramenters;
    join_paramenters.name = name;
    join_paramenters.room = room_name;
    return joiner.join(join_paramenters);
  }
};
} // namespace client::testing

#endif
