#include "http/action_factory.hpp"
#include "http/connection_creator.hpp"
#include "matrix/authentification.hpp"
#include "matrix/client_factory.hpp"
#include "matrix/testing.hpp"
#include "matrix_rooms_factory_adapter.hpp"
#include "server.hpp"
#include "temporary_room/net/client.hpp"
#include "temporary_room/server/application.hpp"
#include <gtest/gtest.h>

namespace {
temporary_room::server::application::options make_application_options() {
  temporary_room::server::application::options result;
  result.port = 0;
  result.matrix_port = matrix::testing::port;
  result.matrix_server = matrix::testing::server;
  result.matrix_target_prefix = matrix::testing::target_prefix;
  return result;
}
} // namespace

TEST(Server, Join) {
  using namespace temporary_room;
  logging::logger logger{"Server"};
  boost::inline_executor executor;
  boost::asio::io_context context;
  server::application::options application_options = make_application_options();
  std::unique_ptr<server::application> application =
      server::application::create(context, application_options);

  // matrix
  matrix::factory matrix_factory;
  http::connection_creator connection_creator_{context};
  http::action_factory action_factory_{connection_creator_};

  http::client_factory http_client_factory_matrix{
      action_factory_, matrix::testing::make_http_server_and_fields()};
  matrix::client_factory matrix_client_factory{matrix_factory,
                                               http_client_factory_matrix};
  matrix::authentification matrix_authentification{http_client_factory_matrix,
                                                   matrix_client_factory};
  auto matrix_client_future = matrix_authentification.register_as_guest();
  context.run();
  context.reset();
  auto matrix_client = matrix_client_future.get();

  // lets start to accept connections
  auto acceptor_done = application->run();

  // temporary_room client
  auto temporary_room_port = application->get_port();
  http::server http_server{"localhost", std::to_string(temporary_room_port)};
  http::fields http_fields{http_server};
  http::client_factory http_client_factory{action_factory_, http_server,
                                           http_fields};
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
            application->close();
            result.get();
          });
  context.run();
  acceptor_done.get();
  join_future.get();
  EXPECT_TRUE(called);
}
