#include <boost/asio/io_context.hpp>
#include <fubble/http_beast/client_module.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <restinio/all.hpp>

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
  auto finished = boost::asio::make_work_guard(*executor->get_io_context());
  bool ran{};
  request->async_run([&finished, &ran](const auto &response) {
    EXPECT_TRUE(response.has_value());
    auto &body = *response.value().body;
    check_version_response(body);
    finished.reset();
    ran = true;
  });
  executor->get_io_context()->run();
  EXPECT_TRUE(ran);
}

TEST_F(HttpBeastClient, PostSync) {
  int port{};
  auto http_server = restinio::run_async(
      restinio::own_io_context(),
      restinio::server_settings_t{}
          .acceptor_post_bind_hook([&port](auto &acceptor) {
            port = acceptor.local_endpoint().port();
          })
          .request_handler([](restinio::request_handle_t request) {
            EXPECT_EQ(request->header().method(), restinio::http_method_post());
            auto request_body = request->body();
            auto request_body_json = nlohmann::json::parse(request_body);
            EXPECT_EQ(request_body_json["pi"], 3.141);
            return request->create_response()
                .set_body("{\"hello\":\"pi\"}")
                .done();
          }),
      1);

  fubble::http2::server server{
      "localhost", std::to_string(port), "", false, {}};
  auto requester = factory->create_requester(server);
  nlohmann::json request_body = {{"pi", 3.141}};
  auto request = requester->post("/", request_body);
  auto response = request->run();
  EXPECT_TRUE(response.has_value());
  auto &response_value = response.value();
  EXPECT_EQ(response_value.code, 200);
  EXPECT_TRUE(response_value.body->contains("hello"));
  EXPECT_EQ((*response_value.body)["hello"], "pi");
}
