#ifndef UUID_5730FA98_0A97_410D_A7A6_A51EED239B26
#define UUID_5730FA98_0A97_410D_A7A6_A51EED239B26

#include "logging/logger.hpp"

namespace signalling {
class logger : public logging::module_logger {
public:
  logger(const std::string &descriptor)
      : logging::module_logger("signalling", descriptor) {}
};
} // namespace signalling

#endif
