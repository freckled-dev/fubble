#ifndef UUID_6223B726_0105_452F_B0AC_13DACAC7A652
#define UUID_6223B726_0105_452F_B0AC_13DACAC7A652

#include <fubble/rtc/video_capability.hpp>
#include <fubble/rtc/video_source.hpp>
#include <memory>
#include <string>

namespace rtc {
class video_device {
public:
  virtual ~video_device() = default;
  virtual void start(const rtc::video::capability &cap) = 0;
  virtual void stop() = 0;
  virtual bool get_started() const = 0;
  virtual std::shared_ptr<rtc::video_source> get_source() const = 0;
  virtual std::string get_id() const = 0;
};
} // namespace rtc

#endif
