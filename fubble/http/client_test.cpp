#include "action_factory.hpp"
#include "client_factory.hpp"
#include "connection_creator.hpp"
#include "fubble/utils/testing.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

TEST(HttpClient, Instance) {
  using namespace http;
  boost::asio::io_context context;
  server server_{utils::testing::server, "http"};
  fields fields{server_};
  connection_creator connection_creator_{context};
  std::shared_ptr<action_factory> action_factory_ =
      std::make_shared<action_factory>(connection_creator_);
  client_factory factory{action_factory_, server_, fields};
  EXPECT_TRUE(factory.create());
}

TEST(HttpClient, Get) {
  using namespace http;
  boost::asio::io_context context;
  boost::inline_executor executor;
  server server_{utils::testing::server, "80"};
  fields fields{server_};
  fields.target_prefix = "/api/matrix/v0/_matrix/client/";
  connection_creator connection_creator_{context};
  std::shared_ptr<action_factory> action_factory_ =
      std::make_shared<action_factory>(connection_creator_);
  client_factory factory{action_factory_, server_, fields};
  auto client_ = factory.create();
  bool called{};
  auto got = client_->get("versions").then(executor, [&](auto result) {
    called = true;
    auto got_result = result.get();
    EXPECT_EQ(got_result.first, boost::beast::http::status::ok);
    EXPECT_TRUE(got_result.second.contains("versions"));
    context.stop();
  });
  context.run();
  EXPECT_TRUE(called);
  EXPECT_NO_THROW(got.get());
}
