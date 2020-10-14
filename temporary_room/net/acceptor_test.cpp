#include "http/action_factory.hpp"
#include "http/client.hpp"
#include "http/connection_creator.hpp"
#include "server.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

TEST(NetAcceptor, Put) {
  logging::logger logger{"NetAcceptor"};
  boost::asio::io_context context;
  temporary_room::net::server::acceptor::config config;
  temporary_room::net::server::acceptor server{context, config};
  auto run_future = server.run();
  const std::string path = "fun/request";
  server.on_request = [&](auto path_, auto /*request*/) {
    EXPECT_EQ(path, path_);
    server.stop();
    return boost::make_ready_future<nlohmann::json>();
  };
  http::server server_{"localhost", std::to_string(server.get_port())};
  http::fields fields_{server_};
  http::connection_creator connection_creator_{context};
  std::shared_ptr<http::action_factory> action_factory_ =
      std::make_shared<http::action_factory>(connection_creator_);
  http::client client{action_factory_, std::make_pair(server_, fields_)};
  auto content = nlohmann::json::object();
  auto get_future = client.put(path, content);
  context.run();
  run_future.get();
  get_future.get();
}
