#include "authentification.hpp"
#include "http/client_factory.hpp"
#include "utils/uuid.hpp"
#include <gtest/gtest.h>

using namespace matrix;

struct Authentifification : ::testing::Test {
  boost::inline_executor executor;
  boost::asio::io_context context;
  http::client::server server_information{"localhost", "8008"};
  http::client::default_fields fields_information{server_information};
  http::client_factory http_client_factory{context, server_information,
                                           fields_information};
};

TEST_F(Authentifification, Instance) {}

TEST_F(Authentifification, RegisterAsGuest) {
  auto client = http_client_factory.create();
  authentification test{*client};
  bool called{};
  auto result = test.register_as_guest().then(executor, [&](auto result) {
    auto got_result = result.get();
    called = true;
    EXPECT_FALSE(got_result.user_id.empty());
    EXPECT_FALSE(got_result.access_token.empty());
    EXPECT_FALSE(got_result.device_id.empty());
    context.stop();
  });
  context.run();
  EXPECT_TRUE(called);
  EXPECT_NO_THROW(result.get());
}

TEST_F(Authentifification, RegisterAsUser) {
  auto client = http_client_factory.create();
  authentification test{*client};
  bool called{};
  auto username = uuid::generate();
  auto password = uuid::generate();
  auto result =
      test.register_(username, password).then(executor, [&](auto result) {
        auto got_result = result.get();
        called = true;
        EXPECT_FALSE(got_result.user_id.empty());
        EXPECT_FALSE(got_result.access_token.empty());
        EXPECT_FALSE(got_result.device_id.empty());
        context.stop();
      });
  context.run();
  EXPECT_TRUE(called);
  EXPECT_NO_THROW(result.get());
}
