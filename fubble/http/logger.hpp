#ifndef UUID_28DC96BB_929C_439D_9B19_B8ADC0909475
#define UUID_28DC96BB_929C_439D_9B19_B8ADC0909475

#include "fubble/utils/logging/logger.hpp"

namespace http {
class logger : public logging::module_logger {
public:
  logger(const std::string &descriptor)
      : logging::module_logger("http", descriptor) {}
};
} // namespace http

#endif
