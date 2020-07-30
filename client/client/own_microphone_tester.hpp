#ifndef UUID_93423F7D_C09C_4D2F_9749_1CEDCABA9AA2
#define UUID_93423F7D_C09C_4D2F_9749_1CEDCABA9AA2

#include <memory>

namespace client {
class loopback_audio;
class own_microphone_tester {
public:
  virtual ~own_microphone_tester() = default;
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual bool get_started() const = 0;

  static std::unique_ptr<own_microphone_tester> create(loopback_audio &audio);
};
} // namespace client

#endif
