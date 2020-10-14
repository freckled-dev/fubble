#include "http/action_factory.hpp"
#include "http/connection_creator.hpp"
#include "version/getter.hpp"
#include "version/server.hpp"
#include <gtest/gtest.h>

TEST(Version, Get) {
  boost::inline_executor executor;
  auto context = std::make_shared<boost::asio::io_context>();
  version::server::config config;
  auto server = version::server::create(context, config);
  http::server server_{"localhost", std::to_string(config.port)};
  http::fields fields_{server_};
  http::connection_creator connection_creator_{*context};
  std::shared_ptr<http::action_factory> action_factory_ =
      std::make_shared<http::action_factory>(connection_creator_);
  auto http_client = std::make_shared<http::client>(
      action_factory_, std::make_pair(server_, fields_));
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
