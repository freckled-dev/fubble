#ifndef UUID_AF395091_9EB1_4734_9A81_1DD8B490F72C
#define UUID_AF395091_9EB1_4734_9A81_1DD8B490F72C

#include "client/test_executor.hpp"
#include "fubble/temporary_room/server/test_server.hpp"
#include "fubble/version/server.hpp"

namespace client::testing {
struct test_services : test_executor {
  temporary_room::testing::server temporary_room_server{context};
  std::unique_ptr<version::server> version_http_server =
      version::server::create(context);
};
} // namespace client::testing

#endif
