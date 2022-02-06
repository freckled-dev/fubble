#ifndef UUID_7E48ABA1_27AF_4041_AF31_8C26D6C15A75
#define UUID_7E48ABA1_27AF_4041_AF31_8C26D6C15A75

#include <fubble/http/fields.hpp>
#include <fubble/http/server.hpp>
#include <fubble/http2/module.hpp>
#include <fubble/utils/testing.hpp>

namespace matrix::testing {
namespace {
static const std::string server = utils::testing::server;
static const std::string port = "80";
static const std::string host = server + ":" + port; // kinda unused
// mainly needed for `/client/versions`
static const std::string target_prefix_not_client = "/api/matrix/v0/_matrix/";
static const std::string target_prefix =
    target_prefix_not_client + "client/r0/";

inline std::pair<http::server, http::fields> make_http_server_and_fields() {
  http::server http_server_matrix{server, port};
  http::fields http_fields{http_server_matrix};
  http_fields.target_prefix = target_prefix;
  return std::make_pair(http_server_matrix, http_fields);
}

inline fubble::http2::server make_http2_server_and_fields() {
  fubble::http2::server result;
  result.host = server;
  result.service = port;
  result.target_prefix = target_prefix;
  return result;
}
} // namespace
} // namespace matrix::testing

#endif
