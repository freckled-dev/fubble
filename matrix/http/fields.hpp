#ifndef UUID_3C68F67D_2678_4722_A891_5F1BF86B4AE5
#define UUID_3C68F67D_2678_4722_A891_5F1BF86B4AE5

#include "server.hpp"
#include <boost/beast/version.hpp>
#include <optional>

namespace matrix::http {
struct fields {
  fields() = default;
  fields(const server &server) { host = server.server + ":" + server.port; }
  std::string host;
  std::string target_prefix{"/_matrix/client/r0/"};
  std::string agent = BOOST_BEAST_VERSION_STRING;
  // TODO support http 1.1
  int version = 10; // http 1.0
  std::optional<std::string> auth_token;
};
} // namespace matrix::http

#endif
