#ifndef UUID_CF1BCC72_006A_4D84_9FAA_0C1588DD2338
#define UUID_CF1BCC72_006A_4D84_9FAA_0C1588DD2338

#include "fubble/utils/export.hpp"

namespace rtc {
namespace google {
class FUBBLE_PUBLIC log_webrtc_to_logging {
public:
  log_webrtc_to_logging();
  ~log_webrtc_to_logging();

  void set_enabled(bool);

protected:
  class sink;
  sink *sink_{};
};
} // namespace google
} // namespace rtc

#endif
