#ifndef UUID_5730FA98_0A97_410D_A7A6_A51EED239B26
#define UUID_5730FA98_0A97_410D_A7A6_A51EED239B26

#include "logging/logger.hpp"

namespace signalling {
class logger : public logging::logger {
public:
  using logging::logger::logger;
};
} // namespace signalling

#endif
