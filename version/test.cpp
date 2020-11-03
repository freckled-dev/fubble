#include "http/action_factory.hpp"
#include "http/connection_creator.hpp"
#include "version/getter.hpp"
#include "version/server.hpp"
#include "version/testing.hpp"
#include <gtest/gtest.h>

namespace {
struct Version : testing::Test {
  boost::inline_executor executor;
  std::shared_ptr<boost::asio::io_context> context =
      std::make_shared<boost::asio::io_context>();
  version::server::config config;
  http::connection_creator connection_creator_{*context};
  std::shared_ptr<http::action_factory> action_factory_ =
      std::make_shared<http::action_factory>(connection_creator_);
};
} // namespace

TEST_F(Version, Get) {
  auto server = version::server::create(context, config);
  auto http_client = std::make_shared<http::client>(
      action_factory_,
      version::testing::make_http_server_and_fields(server->get_port()));
  auto getter = version::getter::create(http_client);
  auto done = getter->get().then(executor, [&](auto result) {
    server.reset();
    auto got = result.get();
    EXPECT_EQ(got.minimum_version, utils::version());
    EXPECT_EQ(got.current_version, utils::version());
  });
  context->run();
  done.get();
}

TEST_F(Version, NoServer) {
  auto http_client = std::make_shared<http::client>(
      action_factory_, version::testing::make_http_server_and_fields(0));
  auto getter = version::getter::create(http_client);
  auto done = getter->get();
  context->run();
  EXPECT_THROW(done.get(), boost::system::system_error);
}

TEST_F(Version, CustomVersion) {
  config.current_version = "2.3.4";
  config.minimum_version = "1.2.3";
  auto server = version::server::create(context, config);
  auto http_client = std::make_shared<http::client>(
      action_factory_,
      version::testing::make_http_server_and_fields(server->get_port()));
  auto getter = version::getter::create(http_client);
  auto done = getter->get().then(executor, [&](auto result) {
    server.reset();
    auto got = result.get();
    EXPECT_EQ(got.minimum_version, config.minimum_version);
    EXPECT_EQ(got.current_version, config.current_version);
  });
  context->run();
  done.get();
}
