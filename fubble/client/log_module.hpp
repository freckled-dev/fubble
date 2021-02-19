#ifndef UUID_78681675_D86C_424A_A390_19991E004C3B
#define UUID_78681675_D86C_424A_A390_19991E004C3B

#include <fubble/utils/export.hpp>
#include <fubble/utils/logging/severity.hpp>

namespace client {
class FUBBLE_PUBLIC log_module {
public:
  struct config {
    bool webrtc{};
    logging::severity severity{logging::severity::debug};
  };
  log_module(const config config_);
};
} // namespace client

#endif
