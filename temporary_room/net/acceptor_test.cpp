#include "http/client.hpp"
#include "server.hpp"
#include <gtest/gtest.h>

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
  http::client client{context, server_, fields_};
  auto content = nlohmann::json::object();
  auto get_future = client.put(path, content);
  context.run();
  run_future.get();
  get_future.get();
}
