#include "acceptor.hpp"
#include "connection.hpp"
#include "connection_creator.hpp"
#include "connector.hpp"
#include "fubble/utils/executor_asio.hpp"
#include <boost/beast/websocket/error.hpp>
#include <boost/thread/executors/executor_adaptor.hpp>
#include <gtest/gtest.h>

struct Websocket : ::testing::Test {
  virtual ~Websocket() = default;
  boost::asio::io_context context;
  boost::executor_adaptor<executor_asio> executor{context};
  websocket::connection_creator connection_creator{context};
  websocket::acceptor::config acceptor_config;
  websocket::acceptor acceptor{context, connection_creator, acceptor_config};
};

TEST_F(Websocket, ConnectionCreation) {
  websocket::connector_creator connector_creator{context, connection_creator};
  acceptor.on_connection.connect(
      [&](auto &connection) { EXPECT_TRUE(connection); });
  bool connected{};
  websocket::connector::config connector_config;
  connector_config.service = std::to_string(acceptor.get_port());
  connector_config.url = "localhost";
  auto connector = connector_creator.create(connector_config);
  connector->connect().then(executor, [&](auto connection) {
    EXPECT_TRUE(connection.get());
    connected = true;
    acceptor.close();
  });
  context.run();
  EXPECT_TRUE(connected);
}

struct WebsocketOpenConnection : Websocket {
  websocket::connector_creator connector_creator{context, connection_creator};
  websocket::connection_ptr first;
  websocket::connection_ptr second;
  WebsocketOpenConnection() {
    acceptor.on_connection.connect(
        [&](auto &result) { first = std::move(result); });
    websocket::connector::config connector_config{
        false, std::to_string(acceptor.get_port()), "localhost"};
    auto connector = connector_creator.create(connector_config);
    connector->connect().then(executor, [&](auto result) {
      second = result.get();
      acceptor.close();
    });
    context.run();
    context.reset();
  }
};
TEST_F(WebsocketOpenConnection, SendReceive) {
  const std::string compare = "hello world";
  bool got_message{};
  first->read().then(executor, [&](auto result) {
    auto message = result.get();
    EXPECT_EQ(compare, message);
    got_message = true;
  });
  second->send(compare);
  context.run();
  EXPECT_TRUE(got_message);
}
TEST_F(WebsocketOpenConnection, SendTwice) {
  bool got_messages{};
  first->read().then(executor, [&](auto result) {
    EXPECT_EQ(result.get(), "hello");
    first->read().then(executor, [&](auto result) {
      got_messages = true;
      EXPECT_EQ(result.get(), "world");
    });
  });
  second->send("hello");
  second->send("world");
  context.run();
  EXPECT_TRUE(got_messages);
}
TEST_F(WebsocketOpenConnection, SendCloseAtOnce) {
  bool got_message{};
  first->read().then(executor, [&](auto) {
    got_message = true;
    // this read is necessary, else the close fragment won't be read
    first->read().then(executor, [&](auto) {});
  });
  second->send("hello");
  second->close();
  context.run();
  EXPECT_TRUE(got_message);
}
TEST_F(WebsocketOpenConnection, Close) {
  bool got_called{};
  first->read().then(executor, [&](auto result) {
    try {
      result.get();
    } catch (boost::system::system_error &error) {
      EXPECT_EQ(error.code(), boost::beast::websocket::error::closed);
      got_called = true;
    }
  });
  second->close();
  context.run();
  EXPECT_TRUE(got_called);
}
