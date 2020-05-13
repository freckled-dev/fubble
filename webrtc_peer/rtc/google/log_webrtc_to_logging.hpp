#ifndef UUID_CF1BCC72_006A_4D84_9FAA_0C1588DD2338
#define UUID_CF1BCC72_006A_4D84_9FAA_0C1588DD2338

namespace rtc {
namespace google {
class log_webrtc_to_logging {
public:
  log_webrtc_to_logging();
  ~log_webrtc_to_logging();

protected:
  class sink;
  sink *sink_{};
};
} // namespace google
} // namespace rtc

#endif
