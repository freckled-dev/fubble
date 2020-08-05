#include "signalling/client/client.hpp"
#include "signalling/client/connection_creator.hpp"
#include "signalling/json_message.hpp"
#include "signalling/server/server.hpp"
#include "utils/executor_asio.hpp"
#include "utils/timer.hpp"
#include "websocket/connection_creator.hpp"
#include "websocket/connector.hpp"
#include <gtest/gtest.h>

TEST(Client, Instance) {
  boost::asio::io_context context;
  executor_asio executor_asio_{context};
  boost::executor_adaptor<executor_asio> boost_executor{executor_asio_};
  websocket::connection_creator websocket_connection_creator{context};
  websocket::connector_creator websocket_connector_creator{
      context, websocket_connection_creator};
  signalling::json_message signalling_json_message;
  signalling::client::connection_creator signalling_client_connection_creator{
      context, boost_executor, signalling_json_message};
  auto client = signalling::client::client::create(
      websocket_connector_creator, signalling_client_connection_creator);
  EXPECT_TRUE(client);
  signalling::client::client::connect_information connect_information;
  signalling::client::client_factory_impl client_factory{
      websocket_connector_creator, signalling_client_connection_creator,
      connect_information};
  utils::one_shot_timer timer{context, std::chrono::milliseconds(10)};
  auto reconnecting_client =
      signalling::client::client::create_reconnecting(client_factory, timer);
  EXPECT_TRUE(reconnecting_client);
}
