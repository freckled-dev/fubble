#include <boost/asio/io_context.hpp>
#include <fubble/http_beast/client_module.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <nlohmann/json.hpp>

struct HttpBeastClient : public ::testing::Test {
  std::shared_ptr<utils::executor_module> executor =
      std::make_shared<utils::executor_module>();
  std::unique_ptr<fubble::http2::factory> factory =
      fubble::http_beast::create_factory(executor);
};

TEST_F(HttpBeastClient, GetSync) {
  fubble::http2::server server{"fubble.io", "http", "/", false, {}};
  auto requester = factory->create_requester(server);
  auto request = requester->get("");
  auto response = request->run();
  EXPECT_TRUE(response.has_error());
  EXPECT_TRUE(response.has_failure());
  EXPECT_FALSE(response.has_exception());
  auto error = response.error();
  EXPECT_EQ(error, fubble::http2::error_code::could_not_parse_json);
}

namespace {
void check_version_response(const nlohmann::json &check) {
  EXPECT_TRUE(check.contains("minimum_version"));
  EXPECT_TRUE(check.contains("current_version"));
}
} // namespace

TEST_F(HttpBeastClient, GetSslSync) {
  fubble::http2::server server{"fubble.io", "https", "/api/", true, {}};
  auto requester = factory->create_requester(server);
  auto request = requester->get("version/v0/");
  auto response = request->run();
  EXPECT_FALSE(response.has_error());
  EXPECT_FALSE(response.has_failure());
  EXPECT_FALSE(response.has_exception());
  EXPECT_TRUE(response.has_value());
  auto &body = *response.value().body;
  check_version_response(body);
}

TEST_F(HttpBeastClient, GetSslAsync) {
  fubble::http2::server server{"fubble.io", "https", "/api/", true, {}};
  auto requester = factory->create_requester(server);
  auto request = requester->get("version/v0/");
  bool called{};
  request->async_run([&called](auto response) {
    EXPECT_TRUE(response.has_value());
    auto &body = *response.value().body;
    check_version_response(body);
    called = true;
  });
  executor->get_io_context()->run();
  EXPECT_TRUE(called);
}
