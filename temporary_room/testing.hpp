#ifndef UUID_9D797151_1E6F_4F32_9BED_C58D093C6C4A
#define UUID_9D797151_1E6F_4F32_9BED_C58D093C6C4A

#include "http/fields.hpp"
#include "http/server.hpp"

namespace temporary_room::testing {
namespace {
inline std::pair<http::server, http::fields> make_http_server_and_fields() {
  http::server http_server_matrix{"localhost", "8009"};
  http::fields http_fields{http_server_matrix};
  return std::make_pair(http_server_matrix, http_fields);
}
} // namespace
} // namespace temporary_room::testing

#endif
