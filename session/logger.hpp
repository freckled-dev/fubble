#ifndef UUID_F3603C5E_8F52_4458_BD94_048555CEC408
#define UUID_F3603C5E_8F52_4458_BD94_048555CEC408

#include "logging/logger.hpp"

namespace session {
class logger : public logging::logger {
public:
  using logging::logger::logger;
};
} // namespace session

#endif
