#ifndef UUID_C2962803_5917_42AB_8D8A_0BE1127565FD
#define UUID_C2962803_5917_42AB_8D8A_0BE1127565FD

#include "matrix/testing.hpp"
#include "temporary_room/server/application.hpp"

namespace temporary_room::testing {
struct server {
  server(boost::asio::io_context &context) {
    application = temporary_room::server::application::create(context, options);
  }

  std::pair<http::server, http::fields> make_http_server_and_fields() {
    http::server http_server{utils::testing::server,
                             std::to_string(application->get_port())};
    http::fields http_fields{http_server};
    http_fields.target_prefix = "/";
    return std::make_pair(http_server, http_fields);
  }

  temporary_room::server::application::options options = [] {
    temporary_room::server::application::options result;
    result.port = 0;
    result.matrix_port = matrix::testing::port;
    result.matrix_server = matrix::testing::server;
    result.matrix_target_prefix = matrix::testing::target_prefix;
    return result;
  }();
  std::unique_ptr<temporary_room::server::application> application;
};
} // namespace temporary_room::testing

#endif
