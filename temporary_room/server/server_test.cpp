#include "matrix/authentification.hpp"
#include "matrix/client_factory.hpp"
#include "matrix/testing.hpp"
#include "matrix_rooms_factory_adapter.hpp"
#include "server.hpp"
#include "temporary_room/net/client.hpp"
#include <gtest/gtest.h>

TEST(Server, Join) {
  using namespace temporary_room;
  logging::logger logger{"Server"};
  boost::inline_executor executor;
  boost::asio::io_context context;
  // matrix
  matrix::factory matrix_factory;
  http::client_factory http_client_factory_matrix{
      context, matrix::testing::make_http_server_and_fields()};
  matrix::client_factory matrix_client_factory{matrix_factory,
                                               http_client_factory_matrix};
  matrix::authentification matrix_authentification{http_client_factory_matrix,
                                                   matrix_client_factory};
  // matrix client for temporary_room::server
  auto matrix_client_server_future =
      matrix_authentification.register_anonymously();
  auto matrix_client_future = matrix_authentification.register_as_guest();
  context.run();
  context.reset();
  auto matrix_client_server = matrix_client_server_future.get();
  auto matrix_client = matrix_client_future.get();
  // temporary_room::server
  net::server::acceptor::config acceptor_config;
  net::server::acceptor acceptor{context, acceptor_config};
  acceptor.listen();
  net::server::server net_server{acceptor};
  server::matrix_rooms_factory_adapter rooms_factory{*matrix_client_server};
  rooms::rooms rooms{rooms_factory};
  server::server server{net_server, rooms};
  // temporary_room client
  http::server http_server{"localhost", std::to_string(acceptor.get_port())};
  http::fields http_fields{http_server};
  http::client_factory http_client_factory{context, http_server, http_fields};
  auto http_client = http_client_factory.create();
  temporary_room::net::client client{*http_client};
  bool called{};
  auto join_future =
      client.join("fun_name", matrix_client->get_user_id())
          .then(executor,
                [&](auto result) {
                  auto id = result.get();
                  BOOST_LOG_SEV(logger, logging::severity::info)
                      << "room_id:" << id;
                  return matrix_client->get_rooms().join_room_by_id(id);
                })
          .unwrap()
          .then(executor, [&](auto result) {
            called = true;
            acceptor.stop();
            result.get();
          });
  // lets start to accept connections
  auto acceptor_done = acceptor.run();
  context.run();
  acceptor_done.get();
  join_future.get();
  EXPECT_TRUE(called);
}
