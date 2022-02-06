#ifndef UUID_6BB2846C_967C_4507_ABE3_A486445A2973
#define UUID_6BB2846C_967C_4507_ABE3_A486445A2973

#include "authentification.hpp"
#include "client_factory.hpp"
#include "error.hpp"
#include "fubble/http/action_factory.hpp"
#include "fubble/http/connection_creator.hpp"
#include "fubble/matrix/testing.hpp"
#include "room.hpp"
#include "users.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <fubble/http_beast/client_module.hpp>
#include <fubble/utils/executor_module.hpp>
#include <gtest/gtest.h>

struct fixture : ::testing::Test {
  const std::shared_ptr<utils::executor_module> executors =
      std::make_shared<utils::executor_module>();
  boost::inline_executor executor;
  boost::asio::io_context context;
  http::connection_creator connection_factory_{context};
  std::shared_ptr<fubble::http2::factory> http_factory_ =
      fubble::http_beast::create_factory(executors);

  matrix::factory room_factory_;
  matrix::client_factory client_factory_{room_factory_, http_factory_};
  matrix::authentification authentification_{
      http_factory_->create_requester(make_server()), client_factory_};

  inline static fubble::http2::server make_server() {
    fubble::http2::server result;
    result.host = "localhost";
    result.service = "http";
    result.target_prefix = "/_matrix/client/r0/";
    return result;
  };

  inline void run_context() {
    context.run();
    context.restart();
  }
  inline std::unique_ptr<matrix::client> create_registered_client() {
    return authentification_.register_anonymously();
  }
  inline std::unique_ptr<matrix::client> create_guest_client() {
    return authentification_.register_as_guest();
  }
  inline void sync_client(matrix::client &client_) {
    client_.sync();
    run_context();
  }
};

#endif
