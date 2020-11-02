#ifndef UUID_2CE437A3_4A8A_451D_B0CD_81AFA972B50D
#define UUID_2CE437A3_4A8A_451D_B0CD_81AFA972B50D

#include "http/fields.hpp"
#include "http/server.hpp"
#include "utils/testing.hpp"

namespace version::testing {
static const std::string server = "localhost";
static const std::string target_prefix = "/";
static const std::string port = "8085";

inline std::pair<http::server, http::fields>
make_http_server_and_fields(int port_) {
  http::server http_server{server, std::to_string(port_)};
  http::fields http_fields{http_server};
  http_fields.target_prefix = target_prefix;
  return std::make_pair(http_server, http_fields);
}
#if 0
inline std::pair<http::server, http::fields> make_http_server_and_fields() {
  http::server http_server{server, port};
  http::fields http_fields{http_server};
  http_fields.target_prefix = target_prefix;
  return std::make_pair(http_server, http_fields);
}
#endif
} // namespace version::testing

#endif
