#ifndef UUID_78681675_D86C_424A_A390_19991E004C3B
#define UUID_78681675_D86C_424A_A390_19991E004C3B

#include <fubble/utils/export.hpp>
#include <fubble/utils/logging/severity.hpp>
#include <memory>

namespace rtc {
namespace google {
class log_webrtc_to_logging;
}
} // namespace rtc
namespace client {
class FUBBLE_PUBLIC log_module {
public:
  struct config {
    bool webrtc{true};
    logging::severity severity{logging::severity::debug};
  };
  log_module(const config config_);
  ~log_module();

protected:
  std::unique_ptr<rtc::google::log_webrtc_to_logging> webrtc_logger;
};
} // namespace client

#endif
