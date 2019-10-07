#include "acceptor.hpp"
#include "connection.hpp"
#include "connection_creator.hpp"
#include "connector.hpp"
#include <boost/beast/websocket/error.hpp>
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
  websocket::connector connector{context, connection_creator};
  spawn([&](auto yield) {
    auto connection = acceptor(yield);
    EXPECT_TRUE(connection);
  });
  bool connected{};
  spawn([&](auto yield) {
    websocket::connector::config connector_config;
    connector_config.service = std::to_string(acceptor.get_port());
    connector_config.url = "localhost";
    auto connection = connector(connector_config, yield);
    EXPECT_TRUE(connection);
    connected = true;
  });
  context.run();
  EXPECT_TRUE(connected);
}

struct WebsocketOpenConnection : Websocket {
  websocket::connector connector{context, connection_creator};
  std::unique_ptr<websocket::connection> first;
  std::unique_ptr<websocket::connection> second;
  WebsocketOpenConnection() {
    spawn([&](auto yield) { first = acceptor(yield); });
    spawn([&](auto yield) {
      websocket::connector::config connector_config{
          std::to_string(acceptor.get_port()), "localhost"};
      second = connector(connector_config, yield);
    });
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

TEST_F(WebsocketOpenConnection, Close) {
  bool got_called{};
  spawn([&](boost::asio::yield_context yield) {
    try {
      first->read(yield);
    } catch (boost::system::system_error &error) {
      EXPECT_EQ(error.code(), boost::beast::websocket::error::closed);
      got_called = true;
    }
  });
  spawn([&](boost::asio::yield_context yield) { second->close(yield); });
  context.run();
  EXPECT_TRUE(got_called);
}
