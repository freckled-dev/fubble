#include "http/action_factory.hpp"
#include "http/connection_creator.hpp"
#include "matrix/authentification.hpp"
#include "matrix/client_factory.hpp"
#include "matrix/testing.hpp"
#include "matrix_rooms_factory_adapter.hpp"
#include "server.hpp"
#include "temporary_room/net/client.hpp"
#include "temporary_room/server/application.hpp"
#include "utils/executor_asio.hpp"
#include "utils/uuid.hpp"
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
struct test_client {
  boost::asio::io_context &context;
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
  std::unique_ptr<matrix::client> matrix_client;

  // temporary_room client
  int temporary_room_port{};
  http::server http_server{"localhost", std::to_string(temporary_room_port)};
  http::fields http_fields{http_server};
  http::client_factory http_client_factory{action_factory_, http_server,
                                           http_fields};
  std::unique_ptr<http::client> http_client = http_client_factory.create();
  temporary_room::net::client client{*http_client};

  test_client(boost::asio::io_context &context, int port)
      : context(context), temporary_room_port{port} {
    auto matrix_client_future = matrix_authentification.register_as_guest();
    while (!matrix_client_future.is_ready()) {
      context.run_one();
      context.reset();
    }
    matrix_client = matrix_client_future.get();
  }
};
struct Server : ::testing::Test {
  logging::logger logger{"Server"};
  boost::inline_executor executor;
  boost::asio::io_context context;
  boost::executor_adaptor<executor_asio> asio_executor{context};
  temporary_room::server::application::options application_options =
      make_application_options();
  std::unique_ptr<temporary_room::server::application> application;

  void instance_application() {
    application = temporary_room::server::application::create(
        context, application_options);
  }

  boost::future<void> join(test_client &client_, std::string name) {
    return client_.client.join(name, client_.matrix_client->get_user_id())
        .then(executor,
              [&](auto result) {
                auto id = result.get();
                BOOST_LOG_SEV(logger, logging::severity::info)
                    << "room_id:" << id;
                return client_.matrix_client->get_rooms().join_room_by_id(id);
              })
        .unwrap()
        .then(executor, [&](auto result) { result.get(); });
  }

  void run_context() {
    context.run();
    context.reset();
  }
};

void ensure_client_rooms_match(const test_client &first,
                               const test_client &second) {
  EXPECT_EQ(first.matrix_client->get_rooms().get_rooms().front()->get_id(),
            second.matrix_client->get_rooms().get_rooms().front()->get_id());
}
} // namespace

TEST_F(Server, Join) {
  instance_application();
  auto acceptor_done = application->run();
  auto client_ =
      std::make_unique<test_client>(context, application->get_port());
  auto join_future =
      join(*client_, "fun_name").then(executor, [&](auto result) {
        application->close();
        result.get();
      });
  context.run();
  acceptor_done.get();
  join_future.get();
}

TEST_F(Server, JoinSame) {
  instance_application();
  auto acceptor_done = application->run();
  auto first_client =
      std::make_unique<test_client>(context, application->get_port());
  auto second_client =
      std::make_unique<test_client>(context, application->get_port());
  std::string room_name = "fun_name";
  auto all_joined = join(*first_client, room_name)
                        .then(executor,
                              [&](auto result) {
                                result.get();
                                return join(*second_client, room_name);
                              })
                        .unwrap()
                        .then(executor, [&](auto result) {
                          application->close();
                          result.get();
                        });
  context.run();
  ensure_client_rooms_match(*first_client, *second_client);
  acceptor_done.get();
  all_joined.get();
}

TEST_F(Server, JoinSameParallel) {
  instance_application();
  auto acceptor_done = application->run();
  auto first_client =
      std::make_unique<test_client>(context, application->get_port());
  auto second_client =
      std::make_unique<test_client>(context, application->get_port());
  std::string room_name = "fun_name";
  auto first_joined = join(*first_client, room_name);
  auto second_joined = join(*second_client, room_name);
  auto all_joined =
      boost::when_all(std::move(first_joined), std::move(second_joined))
          .then(asio_executor, [&](auto result) {
            application->close();
            result.get();
          });
  context.run();
  ensure_client_rooms_match(*first_client, *second_client);
  acceptor_done.get();
  all_joined.get();
}

TEST_F(Server, Restart) {
  {
    temporary_room::server::application::options::login login_;
    login_.password = uuid::generate();
    login_.username = uuid::generate();
    application_options.login_ = login_;
  }
  instance_application();
  auto acceptor_done = application->run();
  auto first_client =
      std::make_unique<test_client>(context, application->get_port());
  std::string room_name = "fun_name";
  auto first_joined =
      join(*first_client, room_name).then(executor, [&](auto result) {
        application->close();
        result.get();
      });
  run_context();
  acceptor_done.get();
  first_joined.get();

  instance_application();
  acceptor_done = application->run();
  auto second_client =
      std::make_unique<test_client>(context, application->get_port());
  auto second_joined =
      join(*second_client, room_name).then(executor, [&](auto result) {
        application->close();
        result.get();
      });
  run_context();
  ensure_client_rooms_match(*first_client, *second_client);
  acceptor_done.get();
  second_joined.get();
}

#if 0
TEST_F(Server, RoomRemoval) {
  instance_application();
  int called{};
  application->get_rooms().on_room_count_changed.connect([&](auto count) {
    if (called == 0) {
      EXPECT_EQ(count, 1);
    }
    if (called == 1) {
      EXPECT_EQ(count, 0);
      application->close();
    }
    ++called;
  });
  auto acceptor_done = application->run();
  auto first_client =
      std::make_unique<test_client>(context, application->get_port());
  std::string room_name = "fun_name";
  auto finished = join(*first_client, room_name)
                      .then(executor,
                            [&](auto result) {
                              result.get();
                              return first_client->matrix_client->set_presence(
                                  // TODO this is an issue. if the user never
                                  // goes online it never will be removed
                                  // TODO this does not work it's too fast. wont
                                  // get synched as online-->offline
                                  matrix::presence::online);
                            })
                      .then(executor,
                            [&](auto result) {
                              result.get();
                              return first_client->matrix_client->set_presence(
                                  matrix::presence::offline);
                            })
                      .unwrap();
  context.run();
  EXPECT_EQ(called, 2);
  acceptor_done.get();
  finished.get();
}
#endif
