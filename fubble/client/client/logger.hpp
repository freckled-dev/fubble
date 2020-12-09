#ifndef UUID_E355128E_FCF2_4AE9_8F55_07E07FA09789
#define UUID_E355128E_FCF2_4AE9_8F55_07E07FA09789

#include "logging/logger.hpp"

namespace client {
class logger : public logging::module_logger {
public:
  logger(const std::string &descriptor)
      : logging::module_logger("client", descriptor) {}
};
} // namespace client

#endif
