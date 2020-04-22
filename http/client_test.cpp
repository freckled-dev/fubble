#include "client_factory.hpp"
#include "utils/testing.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <gtest/gtest.h>

TEST(HttpClient, Instance) {
  using namespace http;
  boost::asio::io_context context;
  server server_{utils::testing::server, "http"};
  fields fields{server_};
  client_factory factory{context, server_, fields};
  EXPECT_TRUE(factory.create());
}

TEST(HttpClient, Get) {
  using namespace http;
  boost::asio::io_context context;
  boost::inline_executor executor;
  server server_{utils::testing::server, "80"};
  fields fields{server_};
  fields.target_prefix = "/api/matrix/v0/_matrix/client/";
  client_factory factory{context, server_, fields};
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
