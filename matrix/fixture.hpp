#ifndef UUID_6BB2846C_967C_4507_ABE3_A486445A2973
#define UUID_6BB2846C_967C_4507_ABE3_A486445A2973

#include "authentification.hpp"
#include "client_factory.hpp"
#include "error.hpp"
#include "http/action_factory.hpp"
#include "http/connection_creator.hpp"
#include "matrix/testing.hpp"
#include "room.hpp"
#include "users.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <gtest/gtest.h>

struct fixture : ::testing::Test {
  boost::inline_executor executor;
  boost::asio::io_context context;
  http::connection_creator connection_factory_{context};
  std::shared_ptr<http::action_factory> action_factory_ =
      std::make_shared<http::action_factory>(connection_factory_);
  http::client_factory http_client_factory{
      action_factory_, matrix::testing::make_http_server_and_fields()};
  matrix::factory room_factory_;
  matrix::client_factory client_factory_{room_factory_, http_client_factory};
  matrix::authentification authentification_{http_client_factory,
                                             client_factory_};

  inline static http::fields make_fields(const http::server &server) {
    http::fields result{server};
    result.target_prefix = "/_matrix/client/r0/";
    return result;
  };
  inline void run_context() {
    context.run();
    context.reset();
  }
  inline std::unique_ptr<matrix::client> create_registered_client() {
    auto client_future = authentification_.register_anonymously();
    run_context();
    return client_future.get();
  }
  inline std::unique_ptr<matrix::client> create_guest_client() {
    auto client_future = authentification_.register_as_guest();
    run_context();
    return client_future.get();
  }
  inline void sync_client(matrix::client &client_) {
    client_.sync();
    run_context();
  }
};

#endif
