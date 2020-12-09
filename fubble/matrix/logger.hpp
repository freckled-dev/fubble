#ifndef UUID_462A443A_5A30_4E50_AFBB_10BDC2F871F9
#define UUID_462A443A_5A30_4E50_AFBB_10BDC2F871F9

#include "fubble/utils/logging/logger.hpp"

namespace matrix {
class logger : public logging::module_logger {
public:
  logger(const std::string &descriptor)
      : logging::module_logger("matrix", descriptor) {}
};
} // namespace matrix

#endif
