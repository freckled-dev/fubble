#include <fubble/http_beast/client_module.hpp>
#include <gtest/gtest.h>

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
