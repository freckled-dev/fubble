#ifndef UUID_9D797151_1E6F_4F32_9BED_C58D093C6C4A
#define UUID_9D797151_1E6F_4F32_9BED_C58D093C6C4A

#include "fubble/http/fields.hpp"
#include "fubble/http/server.hpp"
#include "utils/testing.hpp"

namespace temporary_room::testing {
namespace {
// TODO remove after refactoring to local server instances
inline std::pair<http::server, http::fields> make_http_server_and_fields() {
  http::server http_server{utils::testing::server, "80"};
  http::fields http_fields{http_server};
  http_fields.target_prefix = "/api/temporary_room/v0/";
  return std::make_pair(http_server, http_fields);
}
} // namespace
} // namespace temporary_room::testing

#endif
