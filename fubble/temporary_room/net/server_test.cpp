#include "client.hpp"
#include "fubble/http/action_factory.hpp"
#include "fubble/http/client.hpp"
#include "fubble/http/connection_creator.hpp"
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
  http::connection_creator connection_creator_{context};
  std::shared_ptr<http::action_factory> action_factory_ =
      std::make_shared<http::action_factory>(connection_creator_);
  http::client http_client{action_factory_, std::make_pair(server_, fields_)};
  temporary_room::net::client client{http_client};
  // test
  const std::string room_name = "room_name";
  const std::string room_id = "room_id";
  const std::string user_id = "user_id";
  server.on_join = [&](const std::string &name, const std::string &user_id_) {
    EXPECT_EQ(name, room_name);
    EXPECT_EQ(user_id, user_id_);
    return boost::make_ready_future(room_id);
  };
  bool called{};
  auto join_future =
      client.join(room_name, user_id).then(executor, [&](auto result) {
        auto got_result = result.get();
        EXPECT_EQ(got_result, room_id);
        called = true;
        acceptor.stop();
      });
  context.run();
  run_future.get();
  EXPECT_TRUE(called);
}
