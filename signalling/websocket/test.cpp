#include "acceptor.hpp"
#include "connection.hpp"
#include "connection_creator.hpp"
#include "connector.hpp"
#include <gtest/gtest.h>

static void spawn(std::function<void(boost::asio::yield_context)> to_call) {
  boost::asio::spawn([to_call](auto yield) {
    try {
      to_call(yield);
    } catch (const std::exception &error) {
      std::cout << error.what() << std::endl;
      throw error;
    }
  });
}

struct Websocket : ::testing::Test {
  virtual ~Websocket() = default;
  boost::asio::io_context context;
  websocket::connection_creator connection_creator{context};
  websocket::acceptor::config acceptor_config;
  websocket::acceptor acceptor{context, connection_creator, acceptor_config};
};

TEST_F(Websocket, ConnectionCreation) {
  websocket::connector::config connector_config;
  connector_config.service = std::to_string(acceptor.get_port());
  connector_config.url = "localhost";
  websocket::connector connector{context, connection_creator, connector_config};
  spawn([&](auto yield) {
    auto connection = acceptor(yield);
    EXPECT_TRUE(connection);
  });
  bool connected{};
  spawn([&](auto yield) {
    auto connection = connector(yield);
    EXPECT_TRUE(connection);
    connected = true;
  });
  context.run();
  EXPECT_TRUE(connected);
}

struct WebsocketOpenConnection : Websocket {
  websocket::connector::config connector_config{
      std::to_string(acceptor.get_port()), "localhost"};
  websocket::connector connector{context, connection_creator, connector_config};
  std::unique_ptr<websocket::connection> first;
  std::unique_ptr<websocket::connection> second;
  WebsocketOpenConnection() {
    spawn([&](auto yield) { first = acceptor(yield); });
    spawn([&](auto yield) { second = connector(yield); });
    context.run();
    context.reset();
  }
};
TEST_F(WebsocketOpenConnection, SendReceive) {
  const std::string compare = "hello world";
  bool got_message{};
  spawn([&](boost::asio::yield_context yield) {
    auto message = first->read(yield);
    EXPECT_EQ(compare, message);
    got_message = true;
  });
  spawn(
      [&](boost::asio::yield_context yield) { second->send(yield, compare); });
  context.run();
  EXPECT_TRUE(got_message);
}

