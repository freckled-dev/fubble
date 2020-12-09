#ifndef UUID_610BCBDB_DFB1_4FDC_BD18_56C392AA3530
#define UUID_610BCBDB_DFB1_4FDC_BD18_56C392AA3530

#include "logging/logger.hpp"

namespace temporary_room {
class logger : public logging::module_logger {
public:
  logger(const std::string &descriptor)
      : logging::module_logger("temporary_room", descriptor) {}
};
} // namespace temporary_room

#endif
