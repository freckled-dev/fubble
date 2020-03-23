#include "client_factory.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <gtest/gtest.h>

TEST(HttpClient, Instance) {
  using namespace matrix::http;
  boost::asio::io_context context;
  client::server server{"localhost", "http"};
  client::fields fields{server};
  client_factory factory{context, server, fields};
  EXPECT_TRUE(factory.create());
}

TEST(HttpClient, Get) {
  using namespace matrix::http;
  boost::asio::io_context context;
  boost::inline_executor executor;
  client::server server{"localhost", "8008"};
  client::fields fields{server};
  fields.target_prefix = "/_matrix/client/";
  client_factory factory{context, server, fields};
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
