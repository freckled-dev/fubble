#include "authentification.hpp"
#include "client_factory.hpp"
#include "http/client_factory.hpp"
#include "room_factory.hpp"
#include "utils/uuid.hpp"
#include <gtest/gtest.h>

using namespace matrix;

struct Authentifification : ::testing::Test {
  boost::inline_executor executor;
  boost::asio::io_context context;
  http::client::server server_information{"localhost", "8008"};
  http::client::fields fields_information{server_information};
  http::client_factory http_client_factory{context, server_information,
                                           fields_information};
  room_factory room_factory_;
  client_factory client_factory_{room_factory_, http_client_factory};
  authentification test{http_client_factory, client_factory_};
};

TEST_F(Authentifification, Instance) {}

TEST_F(Authentifification, RegisterAsGuest) {
  bool called{};
  auto result = test.register_as_guest().then(executor, [&](auto result) {
    auto got_result = result.get();
    called = true;
    EXPECT_TRUE(got_result);
    context.stop();
  });
  context.run();
  EXPECT_TRUE(called);
  EXPECT_NO_THROW(result.get());
}

TEST_F(Authentifification, RegisterAsUser) {
  bool called{};
  auto username = uuid::generate();
  auto password = uuid::generate();
  auto result =
      test.register_(username, password).then(executor, [&](auto result) {
        auto got_result = result.get();
        called = true;
        EXPECT_TRUE(got_result);
        context.stop();
      });
  context.run();
  EXPECT_TRUE(called);
  EXPECT_NO_THROW(result.get());
}
