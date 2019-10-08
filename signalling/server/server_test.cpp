#include "client/client.hpp"
#include "client/connection.hpp"
#include "client/connection_creator.hpp"
#include "executor_asio.hpp"
#include "server.hpp"
#include "server/connection_creator.hpp"
#include "signalling/json_message.hpp"
#include "websocket/connection_creator.hpp"
#include <boost/log/keywords/auto_flush.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/thread/executors/executor_adaptor.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <gtest/gtest.h>
#include <thread>

struct Server : testing::Test {
  boost::asio::io_context context;
  boost::executor_adaptor<executor_asio> executor{context};
  websocket::acceptor::config acceptor_config;
  websocket::connection_creator websocket_connection_creator{context};
  websocket::acceptor acceptor{context, websocket_connection_creator,
                               acceptor_config};
  signalling::json_message json_message;
  server::connection_creator server_connection_creator{context, executor,
                                                       json_message};
  server::server server_{executor, acceptor, server_connection_creator};
  websocket::connector connector{context, executor,
                                 websocket_connection_creator};
  client::connection_creator client_connection_creator{context, executor,
                                                       json_message};
  client::client client_{executor, connector, client_connection_creator};
};

TEST_F(Server, SetUp) {
  server_.close();
  context.run();
}

TEST_F(Server, SingleConnect) {
  bool connected{};
  client_.on_connected.connect([&] {
    auto connection = client_.get_connection();
    EXPECT_TRUE(connection);
    connected = true;
    connection->close();
    server_.close();
  });
  client_("localhost", std::to_string(acceptor.get_port()));
  context.run();
  EXPECT_TRUE(connected);
}

TEST_F(Server, CantConnect) {
  server_.close();
  bool called{};
  client_.on_error.connect([&](auto error) {
    called = true;
    EXPECT_EQ(error.code(), boost::asio::error::connection_refused);
  });
  client_("localhost", "");
  context.run();
  EXPECT_TRUE(called);
}
