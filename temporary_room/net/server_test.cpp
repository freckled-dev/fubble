#include "client.hpp"
#include "http/client.hpp"
#include "server.hpp"
#include <gtest/gtest.h>

TEST(NetServer, Join) {
  logging::logger logger{"NetServer"};
  boost::inline_executor executor;
  boost::asio::io_context context;
  // server
  temporary_room::net::server::acceptor::config config;
  temporary_room::net::server::acceptor acceptor{context, config};
  temporary_room::net::server::server server{acceptor};
  auto run_future = acceptor.run();
  // client
  http::server server_{"localhost", std::to_string(acceptor.get_port())};
  http::fields fields_{server_};
  http::client http_client{context, server_, fields_};
  temporary_room::net::client client{http_client};
  // test
  const std::string room_name = "room_name";
  const std::string room_id = "room_id";
  server.on_join = [&](const std::string &name) {
    EXPECT_EQ(name, room_name);
    return boost::make_ready_future(room_id);
  };
  bool called{};
  auto join_future = client.join(room_name).then(executor, [&](auto result) {
    auto got_result = result.get();
    EXPECT_EQ(got_result, room_id);
    called = true;
    acceptor.stop();
  });
  context.run();
  run_future.get();
  EXPECT_TRUE(called);
}
