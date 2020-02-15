#ifndef UUID_E355128E_FCF2_4AE9_8F55_07E07FA09789
#define UUID_E355128E_FCF2_4AE9_8F55_07E07FA09789

#include "logging/logger.hpp"

namespace client {
class logger : public logging::logger {
public:
  using logging::logger::logger;
};
} // namespace client

#endif
