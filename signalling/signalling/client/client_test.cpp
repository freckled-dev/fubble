#include "signalling/client/client.hpp"
#include "signalling/client/connection_creator.hpp"
#include "signalling/json_message.hpp"
#include "signalling/server/application.hpp"
#include "signalling/server/server.hpp"
#include "utils/executor_asio.hpp"
#include "utils/timer.hpp"
#include "websocket/connection_creator.hpp"
#include "websocket/connector.hpp"
#include <gtest/gtest.h>

namespace {
signalling::client::client::connect_information
make_connect_information(const signalling::server::application &server) {
  signalling::client::client::connect_information result;
  result.host = "localhost";
  result.service = std::to_string(server.get_port());
  result.target = "/";
  return result;
}
struct Client : ::testing::Test {
  boost::asio::io_context context;
  std::unique_ptr<signalling::server::application> server =
      signalling::server::application::create(context, 0);
  executor_asio executor_asio_{context};
  boost::executor_adaptor<executor_asio> boost_executor{executor_asio_};
  websocket::connection_creator websocket_connection_creator{context};
  websocket::connector_creator websocket_connector_creator{
      context, websocket_connection_creator};
  signalling::json_message signalling_json_message;
  signalling::client::connection_creator signalling_client_connection_creator{
      context, boost_executor, signalling_json_message};
  std::unique_ptr<signalling::client::client> client =
      signalling::client::client::create(websocket_connector_creator,
                                         signalling_client_connection_creator);
  signalling::client::client::connect_information connect_information =
      make_connect_information(*server);
  signalling::client::client_factory_impl client_factory{
      websocket_connector_creator, signalling_client_connection_creator,
      connect_information};
  utils::one_shot_timer timer{context, std::chrono::milliseconds(10)};
  std::unique_ptr<signalling::client::client> reconnecting_client =
      signalling::client::client::create_reconnecting(client_factory, timer);
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
  client->on_registered.connect([&] {
    EXPECT_TRUE(client->get_registration_token());
    client->close();
  });
  client->on_closed.connect([&] { server->close(); });
  client->connect(key);
  context.run();
}
