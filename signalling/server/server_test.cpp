#include "client/client.hpp"
#include "client/connection.hpp"
#include "client/connection_creator.hpp"
#include "server.hpp"
#include "server/connection_creator.hpp"
#include "signalling/json_message.hpp"
#include "websocket/connection_creator.hpp"
#include <boost/log/keywords/auto_flush.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <gtest/gtest.h>
#include <thread>

struct Server : testing::Test {
  boost::asio::io_context context;
  websocket::acceptor::config acceptor_config;
  websocket::connection_creator websocket_connection_creator{context};
  websocket::acceptor acceptor{context, websocket_connection_creator,
                               acceptor_config};
  signalling::json_message json_message;
  server::connection_creator server_connection_creator{json_message};
  server::server server_{context, acceptor, server_connection_creator};
};

TEST_F(Server, SetUp) {}

TEST_F(Server, SingleConnect) {
  boost::log::add_console_log(std::cout,
                              boost::log::keywords::auto_flush = true);
  websocket::connector connector{context, websocket_connection_creator};
  client::connection_creator client_connection_creator{context, json_message};
  server_.run();
  client::client client_(context, connector, client_connection_creator);
  std::cout << fmt::format("thread:{}", std::this_thread::get_id())
            << std::endl;
  auto connection_future =
      client_("localhost", std::to_string(acceptor.get_port()));
  bool connected{};
  connection_future.then(boost::launch::sync, [&](auto connection_future) {
    std::cout << fmt::format("thread then:{}", std::this_thread::get_id())
              << std::endl;
    try {
      auto connection = connection_future.get();
      std::cout << "after future" << std::endl;
      EXPECT_TRUE(connection);
      connected = true;
      connection->close();
      std::cout << "closed connection" << std::endl;
    } catch (const boost::system::system_error &error) {
      std::cout << "an error occured:" << error.what() << std::endl;
      EXPECT_TRUE(false);
    }
    server_.close();
    std::cout << "closed server" << std::endl;
  });
  context.run();
  std::cout << "end of context.run" << std::endl;
  EXPECT_TRUE(connected);
  std::cout << "end of test" << std::endl;
}
