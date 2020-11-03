#ifndef UUID_5B95FA92_940F_4662_A44F_1C57BBB217F7
#define UUID_5B95FA92_940F_4662_A44F_1C57BBB217F7

#include "logging/logger.hpp"

namespace version {
class logger : public logging::module_logger {
public:
  logger(const std::string &descriptor)
      : logging::module_logger("version", descriptor) {}
};
} // namespace version

#endif
