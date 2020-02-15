#ifndef UUID_60722C22_CC47_468B_9964_3C9BB26A41AC
#define UUID_60722C22_CC47_468B_9964_3C9BB26A41AC

#include "logging/logger.hpp"

namespace rtc {
class logger : public logging::module_logger {
public:
  logger(const std::string &descriptor)
      : logging::module_logger("rtc", descriptor) {}
};
} // namespace rtc

#endif
