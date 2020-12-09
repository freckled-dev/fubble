#ifndef UUID_82E2016D_125F_4D16_AA51_C8D3156A32AE
#define UUID_82E2016D_125F_4D16_AA51_C8D3156A32AE

#include "fubble/utils/logging/logger.hpp"

namespace websocket {
class logger : public logging::module_logger {
public:
  logger(const std::string &descriptor)
      : logging::module_logger("websocket", descriptor) {}
};
} // namespace websocket

#endif
