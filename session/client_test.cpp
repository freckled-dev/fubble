#include "client.hpp"
#include "client_connector.hpp"
#include "matrix/client.hpp"
#include "matrix/testing.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <gtest/gtest.h>

TEST(Client, Connect) {
  boost::asio::io_context io_context;
  boost::inline_executor executor;
  http::client_factory http_client_factory{
      io_context, matrix::testing::make_http_server_and_fields()};
  matrix::factory matrix_factory;
  matrix::client_factory matrix_client_factory{matrix_factory,
                                               http_client_factory};
  matrix::authentification matrix_authentification{http_client_factory,
                                                   matrix_client_factory};
  session::client_factory client_factory;
  session::client_connector connector{client_factory, matrix_authentification};
  bool called{};
  auto connected = connector.connect().then(executor, [&](auto result) {
    called = true;
    result.get();
    io_context.stop();
  });
  io_context.run();
  EXPECT_TRUE(called);
  connected.get();
}
