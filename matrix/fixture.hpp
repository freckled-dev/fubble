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
  matrix::http::fields fields_information{server_information};
  matrix::http::client_factory http_client_factory{context, server_information,
                                                   fields_information};
  matrix::factory room_factory_;
  matrix::client_factory client_factory_{room_factory_, http_client_factory};
  matrix::authentification authentification_{http_client_factory,
                                             client_factory_};

  inline void run_context() {
    context.run();
    context.reset();
  }
};

#endif
