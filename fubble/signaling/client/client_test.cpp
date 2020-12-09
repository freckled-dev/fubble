#include "fubble/signaling/client/client.hpp"
#include "fubble/signaling/client/connection_creator.hpp"
#include "fubble/signaling/json_message.hpp"
#include "fubble/signaling/server/application.hpp"
#include "fubble/signaling/server/server.hpp"
#include "fubble/utils/executor_asio.hpp"
#include "fubble/utils/timer.hpp"
#include "fubble/websocket/connection_creator.hpp"
#include "fubble/websocket/connector.hpp"
#include <gtest/gtest.h>

namespace {
signaling::client::connect_information
make_connect_information(const signaling::server::application &server) {
  signaling::client::connect_information result;
  result.host = "localhost";
  result.service = std::to_string(server.get_port());
  result.target = "/";
  return result;
}
struct Client : ::testing::Test {
  boost::asio::io_context context;
  std::unique_ptr<signaling::server::application> server =
      signaling::server::application::create(context, 0);
  executor_asio executor_asio_{context};
  boost::executor_adaptor<executor_asio> boost_executor{executor_asio_};
  websocket::connection_creator websocket_connection_creator{context};
  websocket::connector_creator websocket_connector_creator{
      context, websocket_connection_creator};
  signaling::json_message signaling_json_message;
  signaling::client::connection_creator signaling_client_connection_creator{
      context, boost_executor, signaling_json_message};
  std::unique_ptr<signaling::client::client> client =
      signaling::client::client::create(websocket_connector_creator,
                                        signaling_client_connection_creator);
  signaling::client::connect_information connect_information =
      make_connect_information(*server);
  signaling::client::factory_impl client_factory{
      websocket_connector_creator, signaling_client_connection_creator,
      connect_information};
  utils::one_shot_timer timer{context, std::chrono::milliseconds(10)};
  std::unique_ptr<signaling::client::client> reconnecting_client =
      signaling::client::client::create_reconnecting(client_factory, timer);
  std::string key{"key"};

  Client() {
    client->set_connect_information(connect_information);
    EXPECT_TRUE(server);
    EXPECT_NE(server->get_port(), 0);
    EXPECT_TRUE(client);
    EXPECT_TRUE(reconnecting_client);
  }
};
void no_error(const boost::system::system_error &error) {
  EXPECT_FALSE(error.code());
}
} // namespace

TEST_F(Client, Instance) {}

TEST_F(Client, Connect) {
  client->on_error.connect(no_error);
  client->on_registered.connect([&] { client->close(); });
  client->on_closed.connect([&] { server->close(); });
  client->connect("token", key);
  context.run();
}
