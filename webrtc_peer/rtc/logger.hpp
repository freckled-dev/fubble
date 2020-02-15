#ifndef UUID_60722C22_CC47_468B_9964_3C9BB26A41AC
#define UUID_60722C22_CC47_468B_9964_3C9BB26A41AC

#include "logging/logger.hpp"

namespace rtc {
class logger : public logging::logger {
public:
  using logging::logger::logger;
};
} // namespace rtc

#endif
