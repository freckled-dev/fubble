#ifndef UUID_1F9AFED9_482F_44D0_9C50_740D4A744285
#define UUID_1F9AFED9_482F_44D0_9C50_740D4A744285

#include <cstdlib>
#include <optional>

namespace utils::testing {
static std::optional<std::string> get_server_from_environment() {
  auto got = std::getenv("FUBBLE_API_HOST");
  if (!got)
    return {};
  return got;
}
static const std::string server =
    get_server_from_environment().value_or("localhost");
} // namespace utils::testing

#endif
