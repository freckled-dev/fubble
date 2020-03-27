#ifndef UUID_6BB2846C_967C_4507_ABE3_A486445A2973
#define UUID_6BB2846C_967C_4507_ABE3_A486445A2973

#include "authentification.hpp"
#include "client_factory.hpp"
#include "error.hpp"
#include "room.hpp"
#include "users.hpp"
#include <boost/thread/executors/inline_executor.hpp>
#include <gtest/gtest.h>

struct fixture : ::testing::Test {
  boost::inline_executor executor;
  boost::asio::io_context context;
  matrix::http::server server_information{"localhost", "8008"};
  matrix::http::fields fields_information = make_fields(server_information);
  matrix::http::client_factory http_client_factory{context, server_information,
                                                   fields_information};
  matrix::factory room_factory_;
  matrix::client_factory client_factory_{room_factory_, http_client_factory};
  matrix::authentification authentification_{http_client_factory,
                                             client_factory_};

  inline static matrix::http::fields
  make_fields(const matrix::http::server &server) {
    matrix::http::fields result{server};
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
