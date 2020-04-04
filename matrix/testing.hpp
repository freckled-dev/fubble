#ifndef UUID_7E48ABA1_27AF_4041_AF31_8C26D6C15A75
#define UUID_7E48ABA1_27AF_4041_AF31_8C26D6C15A75

#include "http/fields.hpp"
#include "http/server.hpp"

namespace matrix::testing {
namespace {
inline std::pair<http::server, http::fields> make_http_server_and_fields() {
  http::server http_server_matrix{"localhost", "8008"};
  http::fields http_fields{http_server_matrix};
  http_fields.target_prefix = "/_matrix/client/r0/";
  return std::make_pair(http_server_matrix, http_fields);
}
} // namespace
} // namespace matrix::testing

#endif
